import React, { useState, useEffect, useCallback } from 'react';
import { ReactSearchAutocomplete } from 'react-search-autocomplete';
import debounce from 'lodash.debounce';
import './App.css';

function App() {
  const [searchTerm, setSearchTerm] = useState('');
  const [suggestions, setSuggestions] = useState([]);

  const fetchSuggestions = async (searchQuery) => {
    console.log(searchQuery);

    if (searchQuery === '')
      return;

    try {
      const response = await fetch(`http://localhost:5000/search?q=${encodeURIComponent(searchQuery)}`);

      const data = await response.json();

      const results = data.map((item, index) => ({
        id: index,
        name: item
      }));

      console.log(results);

      setSuggestions(results); // Assuming the API returns an array of suggestions
    } catch (error) {
      console.error('Error fetching suggestions:', error);
    }
  };

  // Debounced version of fetchSuggestions
  const debouncedFetchSuggestions = useCallback(debounce(fetchSuggestions, 300), []);

  useEffect(() => {
    console.log(`${searchTerm}     ${suggestions.length}`);

    if (searchTerm.trim() === '' && suggestions.length) {
      setSuggestions([]); // Clear suggestions if search term is empty
    } else {
      debouncedFetchSuggestions(searchTerm); // Call debounced function when search term changes
    }
  }, [searchTerm]);

  const handleOnSearch = (search) => {
    setSearchTerm(search);
  };

  const handleOnSelect = (item) => {
    console.log('Selected:', item);
  };

  const handleOnHover = (item) => {
    console.log('Hovered:', item);
  };

  const handleOnFocus = () => {
    console.log('The search input is focused');
  };

  const handleOnClear = () => {
    console.log('The search input is cleared');
    setSuggestions([]);
  };

  return (
    <div className="App">
      <header className="App-header">
        <h1> AutoComplete Search-Bar</h1>
        <div style={{ width: 500, margin: 20 }}>
          <ReactSearchAutocomplete
            items={suggestions}
            onSearch={handleOnSearch}
            onSelect={handleOnSelect}
            onHover={handleOnHover}
            onFocus={handleOnFocus}
            onClear={handleOnClear}
            placeholder="Type to search"
          />
        </div >
      </header>
    </div>
  );
}

export default App;
