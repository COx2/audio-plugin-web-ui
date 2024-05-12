import { useState } from 'react'
import './App.css'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'

function App() {
  // @ts-ignore
  const [count, setCount] = useState(0)

  return (
    <>
      <div style={{display: 'flex', flexDirection: 'column', justifyContent:'center', alignItems:'center'}}>
          <div style={{marginTop: '-10px'}}><GainKnob/></div>
          <div style={{marginTop: '-10px'}}>GAIN</div>
          <div style={{margin: '10px', padding: 0}}></div>
      </div>
      <div>
        <MidiKeyboard/>
      </div>
    </>
  )
}

export default App
