// @ts-ignore
import { React, useState } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
import useWindowFunction from '../native_bridges/useWindowFunction';

const GainKnob = () => {
    // @ts-ignore
    const [value] = useState(0)

    const onSliderChanged = useWindowFunction('onSliderChanged');
    
    return (
        <Knob 
        size={100}  
        angleOffset={220} 
        angleRange={280}
        min={0}
        max={100}
        // @ts-ignore
        onChange={value => {
            const jsonData = {
                sliderName: "gain",
                sliderValue: value,
                sliderRangeMin: 0,
                sliderRangeMax: 100
            };
            onSliderChanged(jsonData)
        }}
        >
            <Arc 
            arcWidth={5}
            color="#FC5A96"
            radius={47.5} 
            />
            <Pointer 
            width={5}
            radius={40}
            type="circle"
            color="000000"
            />
            <Value 
            marginBottom={40} 
            className="value" 
            />
        </Knob>
    );
};

export default GainKnob;