if(0)
{
  var http = require('http');
  var fs = require('fs');
  var index = fs.readFileSync('mashkod.txt');
  console.log("nodejs server runned");
  http.createServer(function (req, res) {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end(index);
    console.log("response");
  }).listen(80);
}

var http = require("http"),
    url = require("url"),
    path = require("path"),
    fs = require("fs")
    port = process.argv[2] || 80;

http.createServer(function(request, response) {
  var uri = url.parse(request.url).pathname
    , filename = path.join(process.cwd(), uri);

  console.log("uri = "+uri);
  var contentTypesByExtension = {
    '.html': "text/html",
    '.css':  "text/css",
    '.js':   "text/javascript",
    '.txt':   "text/plain"
  };
  fs.exists(filename, function(exists) {
    
    if("/mashkod.txt|/hello.html.mem|/hello.html|/hello.js|/test.js|/glUtils.js|/sylvester.js".split("|").indexOf(uri)<0){
      console.log("unk.uri = "+uri);
      exists=false;
    }
    if(!exists) {
      response.writeHead(404, {"Content-Type": "text/plain"});
      response.write("404 Not Found\n");
      response.end();
      return;
    }

    if (fs.statSync(filename).isDirectory()) filename += '/index.html';

    fs.readFile(filename, "binary", function(err, file) {
      if(err) {        
        response.writeHead(500, {"Content-Type": "text/plain"});
        response.write(err + "\n");
        response.end();
        return;
      }

      var headers = {};
      var contentType = contentTypesByExtension[path.extname(filename)];
      if (contentType) headers["Content-Type"] = contentType;
      response.writeHead(200, headers);
      response.write(file, "binary");
      response.end();
    });
  });
}).listen(parseInt(port, 10));

console.log("Static file server running at\n  => http://localhost:" + port + "/\nCTRL + C to shutdown");