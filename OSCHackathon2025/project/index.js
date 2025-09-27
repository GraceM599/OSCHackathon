const express = require("express");
const { exec, execFile } = require("child_process");
const path = require("path");

const app = express();
const PORT = process.env.PORT || 5000;

// Serve the static frontend from "public" folder
app.use(express.static(path.join(__dirname, "public")));

// Run Python greeting script
app.get("/api/greeting", (req, res) => {
  exec(`python3 ${path.join(__dirname, "apps/greeting.py")}`, (err, stdout) => {
    if (err) return res.status(500).json({ error: err.message });
    try {
      res.json(JSON.parse(stdout));
    } catch {
      res.json({ output: stdout.trim() });
    }
  });
});

// Run C++ calendar program
app.get("/api/calendar", (req, res) => {
  execFile(path.join(__dirname, "apps/calendar"), (err, stdout) => {
    if (err) return res.status(500).json({ error: err.message });
    try {
      res.json(JSON.parse(stdout));
    } catch {
      res.json({ output: stdout.trim() });
    }
  });
});

app.listen(PORT, '0.0.0.0', () => {
  console.log(`🚀 Server running on port ${PORT}`);
});