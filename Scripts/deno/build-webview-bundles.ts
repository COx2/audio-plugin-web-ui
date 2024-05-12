#!/usr/bin/env -S deno run --allow-all

/*
This script requires the Deno runtime environment to execute.
Please refer to the official website for instructions on how to install Deno.
https://deno.com/
*/

import { $ } from "https://deno.land/x/dax/mod.ts";
import { fromFileUrl } from "https://deno.land/std/path/mod.ts";

//============================================
export async function buildWebViewBundles() {
    console.log("Deno.cwd(): " + Deno.cwd());

    const __root_directory = fromFileUrl(new URL("../../", import.meta.url));
    Deno.chdir(__root_directory);

    console.log("Deno.cwd(): " + Deno.cwd());
    await $`deno run --allow-all ${__root_directory}/Plugins/DenoGain/WebView/build-webview-bundle.ts`;
    await $`deno run --allow-all ${__root_directory}/Plugins/ReactSynth/WebView/build-webview-bundle.ts`;
}
