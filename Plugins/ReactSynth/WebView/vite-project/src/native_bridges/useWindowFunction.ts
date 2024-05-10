import { useCallback } from 'react';

const useWindowFunction = (functionName) => {
  const boundFunction = useCallback((...args) => {
    if (typeof window[functionName] === 'function') {
      return window[functionName](...args);
    }
    console.error(`Function '${functionName}' not found on window object.`);
  }, [functionName]);

  return boundFunction;
};

export default useWindowFunction;