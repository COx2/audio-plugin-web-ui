import { useFrame } from '@react-three/fiber';
import { FC, useRef } from 'react';
import type { Mesh } from 'three';

export const Cube: FC = () => {
    const cubeRef = useRef<Mesh>(null);
    useFrame(() => {
        const cube = cubeRef.current;
        if (!cube) return;
        cube.rotation.x += 0.01;
        cube.rotation.y += 0.01;
    });
    return (
        <mesh ref={cubeRef}>
            <boxGeometry args={[1, 1, 1]} />
            <meshPhongMaterial color="lightgreen" />
        </mesh>
    );
};