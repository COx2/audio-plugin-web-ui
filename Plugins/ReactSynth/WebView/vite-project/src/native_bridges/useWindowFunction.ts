// @ts-ignore
import { useCallback } from 'react';

// @ts-ignore
const useWindowFunction = (functionName) => {
  // @ts-ignore
  const boundFunction = useCallback((...args) => {
    if (typeof window[functionName] === 'function') {
      // @ts-ignore
      return window[functionName](...args);
    }
    console.error(`Function '${functionName}' not found on window object.`);
  }, [functionName]);

  return boundFunction;
};

export default useWindowFunction;