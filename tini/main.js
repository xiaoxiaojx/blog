const http = require("http");
const { exec } = require("child_process");
const tini = require("./build/Release/addon.node");

const server = http.createServer((req, res) => {
  if (req.url === "/make_zombie") {
    console.log("make_zombie >>>");
    exec("node /test/make_zombie.js", () => {});
    res.end("hello");
  } else if (req.url === "/kill_zombie") {
    console.log("kill_zombie >>>");
    console.log(tini.kill_zombie());
    res.end("hello");
  }
});

server.listen(3000);
