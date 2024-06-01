#!/usr/bin/env -S deno run --allow-all

/*
This script requires the Deno runtime environment to execute.
Please refer to the official website for instructions on how to install Deno.
https://deno.com/
*/

import {$, CommandBuilder} from "https://deno.land/x/dax/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

//============================================
console.log("Deno.cwd(): " + Deno.cwd());


const __file_directory = fromFileUrl(new URL("./", import.meta.url));
const __root_directory = fromFileUrl(new URL("../../../", import.meta.url));
Deno.chdir(__file_directory + "vite-project");

console.log("Deno.cwd(): " + Deno.cwd());

await $`npm install`;
await $`npm run build`;

Deno.chdir(__file_directory + "vite-project/dist");

await $`deno run --allow-write --allow-read --allow-net ${__root_directory}/Tools/deno/mc-zip.ts ../../WebViewBundle.zip ./`
