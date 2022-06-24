#include <Arduino.h>

#include "tcMenuECWebServer.h"

// Generated by tcMenu Designer wrap-ws command on 2022-06-10T08:44:46.951426
// use tcmenu wrap-ws cli command to regenerate 

const char webs_asset_manifest_json[] PROGMEM = {
    "{" \
    "  \"files\": {" \
    "    \"main.css\": \"/static/css/main.2f5f4b49.chunk.css\"," \
    "    \"main.js\": \"/static/js/main.ba88db2b.chunk.js\"," \
    "    \"main.js.map\": \"/static/js/main.ba88db2b.chunk.js.map\"," \
    "    \"runtime-main.js\": \"/static/js/runtime-main.a1d4d675.js\"," \
    "    \"runtime-main.js.map\": \"/static/js/runtime-main.a1d4d675.js.map\"," \
    "    \"static/js/2.70c1c58c.chunk.js\": \"/static/js/2.70c1c58c.chunk.js\"," \
    "    \"static/js/2.70c1c58c.chunk.js.map\": \"/static/js/2.70c1c58c.chunk.js.map\"," \
    "    \"static/js/3.30ad73c1.chunk.js\": \"/static/js/3.30ad73c1.chunk.js\"," \
    "    \"static/js/3.30ad73c1.chunk.js.map\": \"/static/js/3.30ad73c1.chunk.js.map\"," \
    "    \"index.html\": \"/index.html\"," \
    "    \"static/css/main.2f5f4b49.chunk.css.map\": \"/static/css/main.2f5f4b49.chunk.css.map\"," \
    "    \"static/js/2.70c1c58c.chunk.js.LICENSE.txt\": \"/static/js/2.70c1c58c.chunk.js.LICENSE.txt\"," \
    "    \"static/media/large_icon.c9d72f60.png\": \"/static/media/large_icon.c9d72f60.png\"," \
    "    \"static/media/pairingExample.0c2261c4.jpg\": \"/static/media/pairingExample.0c2261c4.jpg\"" \
    "  }," \
    "  \"entrypoints\": [" \
    "    \"static/js/runtime-main.a1d4d675.js\"," \
    "    \"static/js/2.70c1c58c.chunk.js\"," \
    "    \"static/css/main.2f5f4b49.chunk.css\"," \
    "    \"static/js/main.ba88db2b.chunk.js\"" \
    "  ]" \
    "}" \
};
const char webs_static_js_2_70c1c58c_chunk_js_LICENSE_txt[] PROGMEM = {
    "/*" \
    "object-assign" \
    "(c) Sindre Sorhus" \
    "@license MIT" \
    "*/" \
    "" \
    "/** @license React v0.20.2" \
    " * scheduler.production.min.js" \
    " *" \
    " * Copyright (c) Facebook, Inc. and its affiliates." \
    " *" \
    " * This source code is licensed under the MIT license found in the" \
    " * LICENSE file in the root directory of this source tree." \
    " */" \
    "" \
    "/** @license React v17.0.2" \
    " * react-dom.production.min.js" \
    " *" \
    " * Copyright (c) Facebook, Inc. and its affiliates." \
    " *" \
    " * This source code is licensed under the MIT license found in the" \
    " * LICENSE file in the root directory of this source tree." \
    " */" \
    "" \
    "/** @license React v17.0.2" \
    " * react-jsx-runtime.production.min.js" \
    " *" \
    " * Copyright (c) Facebook, Inc. and its affiliates." \
    " *" \
    " * This source code is licensed under the MIT license found in the" \
    " * LICENSE file in the root directory of this source tree." \
    " */" \
    "" \
    "/** @license React v17.0.2" \
    " * react.production.min.js" \
    " *" \
    " * Copyright (c) Facebook, Inc. and its affiliates." \
    " *" \
    " * This source code is licensed under the MIT license found in the" \
    " * LICENSE file in the root directory of this source tree." \
    " */" \
};

const char index_html[] PROGMEM = "<html>\n"
 "    <head><title>Hello</title></head>\n"
 "    <body>\n"
 "        <h1>Hello world</h1>\n"
 "        <p>This is served by tcMenu</p>\n"
 "    </body>\n"
 "</html>";

const char lastModifiedStatic[] = "Wed, 22 Jul 2009 19:15:56 GMT";

void prepareWebServer(TcMenuWebServer& webServer) {
    //webServer.setRootWelome("/index.html");

    webServer.onUrlGet("/index.html", [](tcremote::WebServerResponse &response) {
        response.startHeader();
        response.setHeader(tcremote::WSH_LAST_MODIFIED, lastModifiedStatic);
        response.contentInfo(tcremote::WebServerResponse::HTML_TEXT, sizeof(index_html) -1);
        response.send_P(index_html, sizeof(index_html) - 1);
    });
    webServer.onUrlGet("/asset-manifest.json", [](tcremote::WebServerResponse& response) {
        response.startHeader();
        response.setHeader(tcremote::WSH_CONTENT_ENCODING, "gzip");
        response.setHeader(tcremote::WSH_CACHE_CONTROL, "max-age=2592000");
        response.setHeader(tcremote::WSH_LAST_MODIFIED, lastModifiedStatic);
        response.contentInfo(tcremote::WebServerResponse::JSON_TEXT, sizeof(webs_asset_manifest_json));
        response.send_P((uint8_t*)webs_asset_manifest_json, sizeof  webs_asset_manifest_json);
    });
}


// Add the following forward-definition to your code if you're calling this yourself
// void prepareWebServer(TcMenuWebServer& webServer);
