import { serve } from "https://deno.land/std@0.141.0/http/mod.ts";
import { serveDir } from "https://deno.land/std@0.141.0/http/file_server.ts";

const port = 8080;

console.log(`HTTP server running. Access it at: http://localhost:8080/`);
Deno.serve({ port }, (request) => serveDir(request));