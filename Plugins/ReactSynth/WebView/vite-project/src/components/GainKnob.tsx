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
            if (isNaN(parameterValue)) {
                setValue(0);
            }
            else{
                // @ts-ignore
                const mapped_value = globalThis.mapRange(parameterValue, 0.0, 1.0, 0, 100)
                setValue(mapped_value);
            }
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

        // Update the gain knob value state.
        if(parameterName === "gain")
        {
            if (isNaN(parameterValue)) {
                setValue(0);
            }
            else{
                // @ts-ignore
                const mapped_value = globalThis.mapRange(parameterValue, 0.0, 1.0, 0, 100)
                setValue(mapped_value);
            }
        }
    }

    // console.log("GainKnob: " + value);

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
            background="#180094"
            radius={47.5} 
            percentage={value / 100} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
            />
            <Pointer 
            width={5}
            radius={40}
            type="circle"
            color="000000"
            percentage={value / 100} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
            />
            <Value 
            marginBottom={40} 
            className="value"
            value={value} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
            />
        </Knob>
    );
};

export default GainKnob;