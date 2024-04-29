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

const __root_directory = fromFileUrl(new URL("../../", import.meta.url));
Deno.chdir(__root_directory);

console.log("Deno.cwd(): " + Deno.cwd());

const platform = Deno.build.os;

console.log(platform)

if(platform.toString() === "windows")
{
    await $`cmake --install builds/vs2022 --prefix Release --component WebGain_VST3`;
    await $`cmake --install builds/vs2022 --prefix Release --component DenoGain_VST3`;
}
else if(platform.toString() === "darwin")
{
    await $`cmake --install builds/xcode --prefix Release --component WebGain_VST3`;
    await $`cmake --install builds/xcode --prefix Release --component DenoGain_VST3`;
}
else if(platform.toString() === "linux")
{
    await $`cmake --install builds/ninja-multi --prefix Release --component WebGain_VST3`;
    await $`cmake --install builds/ninja-multi --prefix Release --component DenoGain_VST3`;
}
