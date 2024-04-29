#!/usr/bin/env -S deno run --allow-all

/*
This script requires the Deno runtime environment to execute.
Please refer to the official website for instructions on how to install Deno.
https://deno.com/
*/

import { buildWebViewBundles } from "./build-webview-bundles.ts"
import { buildPlugins } from "./build-plugins.ts";

//============================================
await buildWebViewBundles();
await buildPlugins();
