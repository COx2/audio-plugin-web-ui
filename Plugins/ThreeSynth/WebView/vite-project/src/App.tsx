import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei'
import { Color } from 'three';
import { Perf } from 'r3f-perf'
import { Cube } from './components/Cube';
import './App.css'

function App() {
  const bgColor = new Color("darkgray");
  return (
    <>
      <div className="three-box">
        <Canvas
          camera={{
            fov: 45,
            near: 0.1,
            far: 1000,
            position: [0, 0, 5]
          }}
        >
          <Perf/>
          <color
            attach="background"
            args={[bgColor.r, bgColor.g, bgColor.b]}
          />
          <Cube />
          <OrbitControls />
          <ambientLight args={[0xffffff]} intensity={0.2} />
          <directionalLight position={[0.3, 0.4, 1]} intensity={0.8} />
        </Canvas>
      </div>
    </>
  )
}

export default App
