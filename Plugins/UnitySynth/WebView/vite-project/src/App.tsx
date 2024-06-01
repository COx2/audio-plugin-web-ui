import { Fragment, useState, useCallback, useEffect } from 'react'
import { Unity, useUnityContext } from "react-unity-webgl";
import './App.css'

function App() {
  const [isGameOver, setIsGameOver] = useState(false);
  const [userName, setUserName] = useState();
  const [score, setScore] = useState();

  const {  unityProvider, isLoaded, loadingProgression, sendMessage, addEventListener, removeEventListener } =
    useUnityContext({
      loaderUrl: "unity-binary/builds.loader.js",
      dataUrl: "unity-binary/builds.data",
      frameworkUrl: "unity-binary/builds.framework.js",
      codeUrl: "unity-binary/builds.wasm",
    });

  const handleGameOver = useCallback((userName: any, score: any) => {
    setIsGameOver(true);
    setUserName(userName);
    setScore(score);
  }, []);

  useEffect(() => {
    addEventListener("GameOver", handleGameOver);
    return () => {
      removeEventListener("GameOver", handleGameOver);
    };
  }, [addEventListener, removeEventListener, handleGameOver]);

  function handleClickCommunicate() {
    sendMessage("GameController", "Communicate", 100);
  }

  // We'll round the loading progression to a whole number to represent the
  // percentage of the Unity Application that has loaded.
  const loadingPercentage = Math.round(loadingProgression * 100);

  return (
    <>
      <Fragment>
        <div className="container">
          {isLoaded === false && (
            // We'll conditionally render the loading overlay if the Unity
            // Application is not loaded.
            <div className="loading-overlay">
              <p>Loading... ({loadingPercentage}%)</p>
            </div>
          )}
          <Unity className="unity" unityProvider={unityProvider} />
        </div>
        <div>
          <button onClick={handleClickCommunicate}>Communicate with Unity</button>
          {isGameOver === true && (
            <p>{`Game Over ${userName}! You've scored ${score} points.`}</p>
          )}
        </div>
      </Fragment>
    </>
  )
}

export default App
