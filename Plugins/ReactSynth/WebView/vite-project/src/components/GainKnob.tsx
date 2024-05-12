// @ts-ignore
import { React, useState, useEffect } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
import useWindowFunction from '../native_bridges/useWindowFunction';

const GainKnob = () => {
    // @ts-ignore
    const [value, setValue] = useState(0)

    const onSliderChanged = useWindowFunction('onSliderChanged');
    const getParameterValue = useWindowFunction('getParameterValue');

    useEffect(() => {
        (async() => {
            const parameterValue = await getParameterValue({parameterName: "gain"})
            // @ts-ignore
            const mapped_value = globalThis.mapRange(parameterValue, 0.0, 1.0, 0, 100)
            console.log('gainSlider: ', mapped_value);
            setValue(mapped_value);
          })()
        return () => {
        };
      }, [value, setValue]);

    // @ts-ignore
    globalThis.onParameterChanged = function(jsonData) {
        // This is the function that will be called from C++
        // console.log('JSON from C++: ', jsonData);

        // Access the 'type' property in the JSON data
        const parameterName = jsonData.parameterName;
        const parameterValue = jsonData.parameterValue;

        // Update the HTML label with the message
        if(parameterName === "gain")
        {
            // @ts-ignore
            const mapped_value = globalThis.mapRange(parameterValue, 0.0, 1.0, 0, 100)
            console.log('gainSlider: ', mapped_value);
            
            setValue(mapped_value);
        }
    }

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
            value={value}
            />
        </Knob>
    );
};

export default GainKnob;