import { useState } from 'react'
import './App.css'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'

function App() {
  // @ts-ignore
  const [count, setCount] = useState(0)

  return (
    <>
      <div style={{display: 'flex',  justifyContent:'center', alignItems:'center'}}>
        <GainKnob/>
      </div>
      <div>
        <MidiKeyboard/>
      </div>
    </>
  )
}

export default App
