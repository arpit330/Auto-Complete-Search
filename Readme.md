# Auto-Complete Search Service

This project is a high-performance auto-complete search service using a **C++ Trie** for search algorithms, integrated with a **Node.js backend** and a **React.js frontend**. The backend handles Trie-based searches, and the frontend provides the user interface for interacting with the search service.

## Prerequisites

Ensure you have the following installed:

- **Node.js** (v14 or higher)
- **npm** (comes with Node.js)
- **g++** or **clang++** (to compile the C++ addon)
- **CMake** (to build the C++ Trie Node addon)

---

## 1. Backend Setup

### Step 1: Build the C++ Trie Node Addon

1. Navigate to the **backend** folder:

   ```bash
   cd Backend

   ```

2. Build Node Addon

```bash
npx node-gyp clean
npx node-gyp configure
npx node-gyp build
```

This will compile the C++ Trie and generate the required Node.js addon.

### Step 2: Install Node.js Dependencies

```
npm install
```

### Step 3: Start the Backend Server

Use nodemon to start the backend server:

```
nodemon index.js
```

## 1. Frontend Setup

### Step 1: Navigate to the Frontend Directory

```
cd ../Frontend
```

### Step 2: Install Frontend Dependencies

Install the required dependencies for the React frontend:

```
npm install
```

### Step 3: Start the Frontend Server

Start the React development server:

```
npm start
```
