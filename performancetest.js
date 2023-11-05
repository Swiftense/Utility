const fs = require('fs');
const { performance } = require('perf_hooks');

const inputFilePath = 'bigtest.json'; // Replace with the actual path to your JSON input file
const outputFilePath = 'out2.json';  // Replace with the path to your output JSON file

// Start the timer
const startTime = performance.now();

// Read the JSON file
fs.readFile(inputFilePath, 'utf8', (err, data) => {
    if (err) {
        console.error('Error reading the file:', err);
        return;
    }

    // Parse the JSON data
    const jsonData = JSON.parse(data);

    // Stop the timer for reading and parsing
    const readEndTime = performance.now();

    // Calculate the time taken for reading and parsing in milliseconds
    const readTimeTaken = readEndTime - startTime;

    // Write the JSON data to the output file
    fs.writeFile(outputFilePath, JSON.stringify(jsonData, null, 2), (writeErr) => {
        if (writeErr) {
            console.error('Error writing the file:', writeErr);
            return;
        }

        // Stop the timer for writing
        const writeEndTime = performance.now();

        // Calculate the time taken for writing in milliseconds
        const writeTimeTaken = writeEndTime - readEndTime;

        console.log('Time taken to read and parse the JSON file (in milliseconds):', readTimeTaken);
        console.log('Time taken to write the JSON data to the output file (in milliseconds):', writeTimeTaken);
    });
});
