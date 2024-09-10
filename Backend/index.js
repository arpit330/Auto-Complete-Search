const path = require('path');
const addon = require('./build/Release/trie_search');
const express = require('express');
const cors = require('cors');

const app = express();
app.use(cors());

const port = 5000;
const filePath = path.join(__dirname, 'src/resources/words');

addon.buildTrie(filePath, true);

app.get('/search', (req, res) => {
    const query = req.query.q.toLowerCase();
    console.log(query);
    const results = addon.prefixSearch(query);

    res.json(results.slice(0, 10));
});

app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});

