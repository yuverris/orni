#include "../orni_http_server.hpp"
#include <fstream>

std::string_view defaultIndex = ""
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"</head>\n"
"<body>\n"
"<title>Orni</title>\n"
"<div class=\"container\">\n"
"<h1>Congrats your Orni app works fine</h1>\n"
"<p>edit ./index.html or add more files to begin!</p>\n"
"</div>\n"
"<style>\n"
"body {\n\tcolor: #0070f3;\n\ttext-align: center;\n}\n .container {\n\ttext-align: center;\n\tfont-family: -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, Oxygen, Ubuntu,"
    "Cantarell, Fira Sans, Droid Sans, Helvetica Neue, sans-serif;\n}"
"</style>\n"
"</body>\n"
"</html>\n"
"";




void localServer() {
    orni::HttpServer app;
    app.route("/", [&](orni::Request&& req, orni::Response&& res) {
            std::ifstream index("./index.html");
            std::string renderPage;
            if (!index) {
                renderPage = "<h1>Hello from orni!<h1>\n"
                    "looks like that index.html doesn't exist on the current directory\n"
                    "add it manually or run `init` command\n";
            } else {
                std::string ss;
                while (std::getline(index, ss)) {
                    renderPage += ss;
                }
            }
            res.set("Content-Type", "text/html");
            res.send(renderPage);
            res.dump();
    });
    app.route("/:file/", [&](orni::Request&& req, orni::Response&& res) {
            std::string fileName = req.Params["file"];
            std::fstream file;
            std::stringstream filepath;
            filepath << "./" << fileName;
            file.open(filepath.str().c_str() , std::ios::in);
            std::string exRes;
            if (!file) {
                exRes = "file not found";
            } else {
                std::string content;
                std::string cs;
                while (std::getline(file, cs)) {
                    content += cs;
                }
                exRes = std::move(content);
            }
            res.send(exRes);
            res.dump();
    });
    app.run(1234);
}




void initCmd() {
    std::fstream index;
    index.open("./index.html", std::ios::out);
    if (index) {
        index << defaultIndex;
        index.close();
        std::cout << "created ./index.html file ! now run `start` to start the web server!\n";
    } else if (!index) {
        std::cout << "failed to created ./index.html\n";
    }
}


int main(int argc, char ** argv) {
    std::string arg(argv[1]);
    if (arg.empty()) {
        std::cout << "missing argument !\n";
    } else if (arg == "init") {
        initCmd();
    } else if (arg == "start") {
        localServer();
    }
}
