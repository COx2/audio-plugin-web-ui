import { Fragment, useEffect, useRef, useState } from "react";
import { Unity, useUnityContext } from "react-unity-webgl";

function UnityTest() {
  function handleCacheControl(url: string) {
    console.log(`Cache control for ${url}`);
    return "no-cache";
  }

  const {
    loadingProgression,
    unityProvider,
    isLoaded,
    requestFullscreen,
    requestPointerLock,
    sendMessage,
    initialisationError,
    addEventListener,
    removeEventListener,
    takeScreenshot,
    unload,
    UNSAFE__unityInstance,
  } = useUnityContext({
    codeUrl: `/unity-binary/builds.wasm`,
    dataUrl: `/unity-binary/builds.data`,
    frameworkUrl: `/unity-binary/builds.framework.js`,
    loaderUrl: `/unity-binary/builds.loader.js`,
    webglContextAttributes: {
      preserveDrawingBuffer: true,
    },
    cacheControl: handleCacheControl,
  });

  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [screenshots, setScreenshots] = useState<string[]>([]);
  const [canvasWidth, setCanvasWidth] = useState(500);

  function handleClickRequestFullScreen() {
    requestFullscreen(true);
  }

  function handleClickRequestPointerLock() {
    requestPointerLock();
  }

  function handleClickUnload() {
    unload();
  }

  function handleClickSetRandomCanvasWidth() {
    setCanvasWidth(Math.floor(Math.random() * 500) + 250);
  }

  function handleClickSetLogText() {
    sendMessage(
      "Persistent",
      "SetLogText",
      "Hello World, the time is " + new Date().toISOString()
    );
  }

  function handleClickLogCanvasRefToConsole() {
    console.log("Canvas Reference", { canvasRef: canvasRef.current });
  }

  function handleClickLogUnityInstanceRefToConsole() {
    console.log("Unity Instance Reference", { UNSAFE__unityInstance });
  }

  function handleClickTakeScreenshot() {
    const screenshot = takeScreenshot("image/jpg", 1);
    if (screenshot) {
      setScreenshots([...screenshots, screenshot]);
    }
  }

  useEffect(() => {
    function logParametersToConsole(...parameters: any[]) {
      console.log({ parameters });
    }

    addEventListener("ClickedTestButton", logParametersToConsole);
    addEventListener("ClickedStringTestButton", logParametersToConsole);
    addEventListener("ClickedNumberTestButton", logParametersToConsole);
    addEventListener("ClickedNumbersTestButton", logParametersToConsole);
    addEventListener("ClickedBoolTestButton", logParametersToConsole);
    addEventListener("ClickedObjectTestButton", logParametersToConsole);
    return () => {
      removeEventListener("ClickedTestButton", logParametersToConsole);
      removeEventListener("ClickedStringTestButton", logParametersToConsole);
      removeEventListener("ClickedNumberTestButton", logParametersToConsole);
      removeEventListener("ClickedNumbersTestButton", logParametersToConsole);
      removeEventListener("ClickedBoolTestButton", logParametersToConsole);
      removeEventListener("ClickedObjectTestButton", logParametersToConsole);
    };
  }, [addEventListener, removeEventListener]);

  return (
    <Fragment>
      <h2>Unity View</h2>
      <Unity
        unityProvider={unityProvider}
        style={{
          border: "1px solid red",
          height: 400,
          width: canvasWidth,
        }}
        devicePixelRatio={window.devicePixelRatio}
        disabledCanvasEvents={["dragstart"]}
        ref={canvasRef}
        id="my-custom-cancas-id"
      />
      <p>
        Loading progression: <code>{loadingProgression}</code>
        <br />
        Loaded: <code>{isLoaded ? "YES" : "NO"}</code>
        <br />
        Error: <code>{initialisationError || "NONE"}</code>
        <br />
        References:
        <button onClick={handleClickLogCanvasRefToConsole}>
          Log Canvas Ref to Console
        </button>
        <button onClick={handleClickLogUnityInstanceRefToConsole}>
          (Unsafe) Log Unity Instance Ref to Console
        </button>
        <br />
        Actions:
        <button onClick={handleClickRequestFullScreen}>
          Request Fullscreen
        </button>
        <button onClick={handleClickRequestPointerLock}>
          Request Pointer Lock
        </button>
        <button onClick={handleClickUnload}>Unload</button>
        <br />
        Screenshots:
        <button onClick={handleClickTakeScreenshot}>Take Screenshot</button>
        {screenshots.map((screenshot, index) => (
          <img key={index} src={screenshot} height={50} alt="Screenshot" />
        ))}
        <br />
        Communication:
        <button onClick={handleClickSetLogText}>Set Log Text</button>
        <br />
        Other:
        <button onClick={handleClickSetRandomCanvasWidth}>
          Set Random Canvas Width
        </button>
      </p>
    </Fragment>
  );
}

export { UnityTest };
