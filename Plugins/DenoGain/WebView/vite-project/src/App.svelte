<script lang="ts">
  import svelteLogo from './assets/svelte.svg'
  import ParameterController, {setupParameterController, onParameterChanged} from './lib/ParameterController.svelte'
  import { onMount } from 'svelte';
  onMount(() => {
    // Custom codes...
    ParameterController:setupParameterController();

    // Register C++ callback function as global.
    (window as any).onParameterChanged = onParameterChanged;

    // Invoke C++ callback for requesting initial update.
    (window as any).onInitialUpdate();
  });
</script>

<main>
  <h1>DenoGain</h1>
  <img src="/vite-deno.svg" alt="Vite with Deno" class="logo" />
  <div>
    <a href="https://vitejs.dev" target="_blank" rel="noreferrer"> 
      <img src="/vite.svg" class="logo" alt="Vite Logo" />
    </a>
    <a href="https://svelte.dev" target="_blank" rel="noreferrer"> 
      <img src={svelteLogo} class="logo svelte" alt="Svelte Logo" />
    </a>
  </div>
  <h1>Vite + Svelte</h1>
  <div class="card">
    <ParameterController />
  </div>
</main>

<style>
  .logo {
    height: 6em;
    padding: 1.5em;
    will-change: filter;
  }
  .logo:hover {
    filter: drop-shadow(0 0 2em #646cffaa);
  }
  .logo.svelte:hover {
    filter: drop-shadow(0 0 2em #ff3e00aa);
  }
</style>