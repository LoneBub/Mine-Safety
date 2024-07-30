const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
app.use(cors());

app.get("/data1.json", (req, res) => {
  res.sendFile(path.join(__dirname, "data1.json"));
});

app.listen(8000, () => {
  console.log("Server is running on port 8000");
});
