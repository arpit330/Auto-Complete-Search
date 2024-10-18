const path = require('path');
const addon = require('./build/Release/trie_search');
const express = require('express');
const cors = require('cors');
const redis = require('redis');
const { log } = require('console');

const redisClient = redis.createClient({
    url: 'redis://localhost:6379'
});

redisClient.on('error', (err) => {
    console.log('error connecting to redis client: ', err);
});

redisClient.on("ready", () => {
    console.log("Redis connected");
});

redisClient.connect();

const app = express();
app.use(cors());

app.use((req, res, next) => {
    const start = performance.now();

    res.on('finish', () => {
        const duration = performance.now() - start;
        console.log(`Request to ${req.method} ${req.url} took ${duration.toFixed(2)}ms`);
    });

    next();
});

const port = 5000;
const filePath = path.join(__dirname, 'src/resources/words');

addon.buildTrie(filePath, true);

// Middleware to check the Redis cache
async function cacheLookup(req, res, next) {
    const query = req.query.q.toLowerCase();

    try {
        let cachedResult = await redisClient.get(query);

        if (cachedResult) {
            console.log('Cache hit for query:', query);
            return res.send(JSON.parse(cachedResult));  // Send cached result
        }

        // If not found in cache, move to the next middleware/handler
        console.log('Cache miss for query:', query);
        next();
    }
    catch (err) {
        console.error('Redis GET error: ', err);
        return next();
    }

}

app.get('/search', cacheLookup, (req, res) => {
    const query = req.query.q.toLowerCase();
    console.log(query);

    const results = addon.prefixSearch(query);

    // Limit results to 10 items
    const limitedResults = results.slice(0, 10);

    // Cache the result in Redis with an expiration time (1 hour)
    redisClient.set(query, JSON.stringify(limitedResults), {
        EX: 3600,
    });

    res.send(limitedResults);
});

app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});

