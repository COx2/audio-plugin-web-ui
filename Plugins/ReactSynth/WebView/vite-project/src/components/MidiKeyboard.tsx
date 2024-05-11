// @ts-ignore
import React from 'react';
// @ts-ignore
import { Piano, KeyboardShortcuts, MidiNumbers } from 'react-piano';
import 'react-piano/dist/styles.css';
import useWindowFunction from '../native_bridges/useWindowFunction';

const MidiKeyboard = () => {
    
    const firstNote = MidiNumbers.fromNote('c4');
    const lastNote = MidiNumbers.fromNote('f6');
    const keyboardShortcuts = KeyboardShortcuts.create({
        firstNote: firstNote,
        lastNote: lastNote,
        keyboardConfig: KeyboardShortcuts.HOME_ROW,
    });

    const onMidiNoteOn = useWindowFunction('onMidiNoteOn');
    const onMidiNoteOff = useWindowFunction('onMidiNoteOff');

    return (
        <Piano
        noteRange={{ first: firstNote, last: lastNote }}
        playNote={(midiNumber: number) => {
            // Play a given note - see notes below
            onMidiNoteOn(1, midiNumber)
        }}
        stopNote={(midiNumber: number) => {
            // Stop playing a given note - see notes below
            onMidiNoteOff(1, midiNumber)
        }}
        width={1000}
        keyboardShortcuts={keyboardShortcuts}
        />
    );
};

export default MidiKeyboard;