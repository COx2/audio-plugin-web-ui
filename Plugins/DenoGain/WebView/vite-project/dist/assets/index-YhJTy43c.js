var K=Object.defineProperty;var Y=(e,t,n)=>t in e?K(e,t,{enumerable:!0,configurable:!0,writable:!0,value:n}):e[t]=n;var I=(e,t,n)=>(Y(e,typeof t!="symbol"?t+"":t,n),n);(function(){const t=document.createElement("link").relList;if(t&&t.supports&&t.supports("modulepreload"))return;for(const a of document.querySelectorAll('link[rel="modulepreload"]'))r(a);new MutationObserver(a=>{for(const o of a)if(o.type==="childList")for(const s of o.addedNodes)s.tagName==="LINK"&&s.rel==="modulepreload"&&r(s)}).observe(document,{childList:!0,subtree:!0});function n(a){const o={};return a.integrity&&(o.integrity=a.integrity),a.referrerPolicy&&(o.referrerPolicy=a.referrerPolicy),a.crossOrigin==="use-credentials"?o.credentials="include":a.crossOrigin==="anonymous"?o.credentials="omit":o.credentials="same-origin",o}function r(a){if(a.ep)return;a.ep=!0;const o=n(a);fetch(a.href,o)}})();function h(){}function T(e){return e()}function B(){return Object.create(null)}function E(e){e.forEach(T)}function k(e){return typeof e=="function"}function A(e,t){return e!=e?t==t:e!==t||e&&typeof e=="object"||typeof e=="function"}let b;function D(e,t){return e===t?!0:(b||(b=document.createElement("a")),b.href=t,e===b.href)}function Q(e){return Object.keys(e).length===0}function d(e,t){e.appendChild(t)}function R(e,t,n){e.insertBefore(t,n||null)}function N(e){e.parentNode&&e.parentNode.removeChild(e)}function m(e){return document.createElement(e)}function W(e){return document.createTextNode(e)}function L(){return W(" ")}function P(e,t,n){n==null?e.removeAttribute(t):e.getAttribute(t)!==n&&e.setAttribute(t,n)}function X(e){return Array.from(e.childNodes)}let x;function w(e){x=e}function Z(){if(!x)throw new Error("Function called outside component initialization");return x}function ee(e){Z().$$.on_mount.push(e)}const v=[],F=[];let y=[];const M=[],te=Promise.resolve();let O=!1;function ne(){O||(O=!0,te.then(j))}function S(e){y.push(e)}const C=new Set;let _=0;function j(){if(_!==0)return;const e=x;do{try{for(;_<v.length;){const t=v[_];_++,w(t),re(t.$$)}}catch(t){throw v.length=0,_=0,t}for(w(null),v.length=0,_=0;F.length;)F.pop()();for(let t=0;t<y.length;t+=1){const n=y[t];C.has(n)||(C.add(n),n())}y.length=0}while(v.length);for(;M.length;)M.pop()();O=!1,C.clear(),w(e)}function re(e){if(e.fragment!==null){e.update(),E(e.before_update);const t=e.dirty;e.dirty=[-1],e.fragment&&e.fragment.p(e.ctx,t),e.after_update.forEach(S)}}function ae(e){const t=[],n=[];y.forEach(r=>e.indexOf(r)===-1?t.push(r):n.push(r)),n.forEach(r=>r()),y=t}const V=new Set;let oe;function q(e,t){e&&e.i&&(V.delete(e),e.i(t))}function ie(e,t,n,r){if(e&&e.o){if(V.has(e))return;V.add(e),oe.c.push(()=>{V.delete(e),r&&(n&&e.d(1),r())}),e.o(t)}else r&&r()}function le(e){e&&e.c()}function z(e,t,n){const{fragment:r,after_update:a}=e.$$;r&&r.m(t,n),S(()=>{const o=e.$$.on_mount.map(T).filter(k);e.$$.on_destroy?e.$$.on_destroy.push(...o):E(o),e.$$.on_mount=[]}),a.forEach(S)}function H(e,t){const n=e.$$;n.fragment!==null&&(ae(n.after_update),E(n.on_destroy),n.fragment&&n.fragment.d(t),n.on_destroy=n.fragment=null,n.ctx=[])}function se(e,t){e.$$.dirty[0]===-1&&(v.push(e),ne(),e.$$.dirty.fill(0)),e.$$.dirty[t/31|0]|=1<<t%31}function G(e,t,n,r,a,o,s=null,p=[-1]){const f=x;w(e);const i=e.$$={fragment:null,ctx:[],props:o,update:h,not_equal:a,bound:B(),on_mount:[],on_destroy:[],on_disconnect:[],before_update:[],after_update:[],context:new Map(t.context||(f?f.$$.context:[])),callbacks:B(),dirty:p,skip_bound:!1,root:t.target||f.$$.root};s&&s(i.root);let $=!1;if(i.ctx=n?n(e,t.props||{},(l,u,...g)=>{const c=g.length?g[0]:u;return i.ctx&&a(i.ctx[l],i.ctx[l]=c)&&(!i.skip_bound&&i.bound[l]&&i.bound[l](c),$&&se(e,l)),u}):[],i.update(),$=!0,E(i.before_update),i.fragment=r?r(i.ctx):!1,t.target){if(t.hydrate){const l=X(t.target);i.fragment&&i.fragment.l(l),l.forEach(N)}else i.fragment&&i.fragment.c();t.intro&&q(e.$$.fragment),z(e,t.target,t.anchor),j()}w(f)}class U{constructor(){I(this,"$$");I(this,"$$set")}$destroy(){H(this,1),this.$destroy=h}$on(t,n){if(!k(n))return h;const r=this.$$.callbacks[t]||(this.$$.callbacks[t]=[]);return r.push(n),()=>{const a=r.indexOf(n);a!==-1&&r.splice(a,1)}}$set(t){this.$$set&&!Q(t)&&(this.$$.skip_bound=!0,this.$$set(t),this.$$.skip_bound=!1)}}const ce="4";typeof window<"u"&&(window.__svelte||(window.__svelte={v:new Set})).v.add(ce);const ue="data:image/svg+xml,%3csvg%20xmlns='http://www.w3.org/2000/svg'%20xmlns:xlink='http://www.w3.org/1999/xlink'%20aria-hidden='true'%20role='img'%20class='iconify%20iconify--logos'%20width='26.6'%20height='32'%20preserveAspectRatio='xMidYMid%20meet'%20viewBox='0%200%20256%20308'%3e%3cpath%20fill='%23FF3E00'%20d='M239.682%2040.707C211.113-.182%20154.69-12.301%20113.895%2013.69L42.247%2059.356a82.198%2082.198%200%200%200-37.135%2055.056a86.566%2086.566%200%200%200%208.536%2055.576a82.425%2082.425%200%200%200-12.296%2030.719a87.596%2087.596%200%200%200%2014.964%2066.244c28.574%2040.893%2084.997%2053.007%20125.787%2027.016l71.648-45.664a82.182%2082.182%200%200%200%2037.135-55.057a86.601%2086.601%200%200%200-8.53-55.577a82.409%2082.409%200%200%200%2012.29-30.718a87.573%2087.573%200%200%200-14.963-66.244'%3e%3c/path%3e%3cpath%20fill='%23FFF'%20d='M106.889%20270.841c-23.102%206.007-47.497-3.036-61.103-22.648a52.685%2052.685%200%200%201-9.003-39.85a49.978%2049.978%200%200%201%201.713-6.693l1.35-4.115l3.671%202.697a92.447%2092.447%200%200%200%2028.036%2014.007l2.663.808l-.245%202.659a16.067%2016.067%200%200%200%202.89%2010.656a17.143%2017.143%200%200%200%2018.397%206.828a15.786%2015.786%200%200%200%204.403-1.935l71.67-45.672a14.922%2014.922%200%200%200%206.734-9.977a15.923%2015.923%200%200%200-2.713-12.011a17.156%2017.156%200%200%200-18.404-6.832a15.78%2015.78%200%200%200-4.396%201.933l-27.35%2017.434a52.298%2052.298%200%200%201-14.553%206.391c-23.101%206.007-47.497-3.036-61.101-22.649a52.681%2052.681%200%200%201-9.004-39.849a49.428%2049.428%200%200%201%2022.34-33.114l71.664-45.677a52.218%2052.218%200%200%201%2014.563-6.398c23.101-6.007%2047.497%203.036%2061.101%2022.648a52.685%2052.685%200%200%201%209.004%2039.85a50.559%2050.559%200%200%201-1.713%206.692l-1.35%204.116l-3.67-2.693a92.373%2092.373%200%200%200-28.037-14.013l-2.664-.809l.246-2.658a16.099%2016.099%200%200%200-2.89-10.656a17.143%2017.143%200%200%200-18.398-6.828a15.786%2015.786%200%200%200-4.402%201.935l-71.67%2045.674a14.898%2014.898%200%200%200-6.73%209.975a15.9%2015.9%200%200%200%202.709%2012.012a17.156%2017.156%200%200%200%2018.404%206.832a15.841%2015.841%200%200%200%204.402-1.935l27.345-17.427a52.147%2052.147%200%200%201%2014.552-6.397c23.101-6.006%2047.497%203.037%2061.102%2022.65a52.681%2052.681%200%200%201%209.003%2039.848a49.453%2049.453%200%200%201-22.34%2033.12l-71.664%2045.673a52.218%2052.218%200%200%201-14.563%206.398'%3e%3c/path%3e%3c/svg%3e";function de(e){let t;return{c(){t=m("div"),t.innerHTML='<p>Gain [<span id="gainValueLabel">50</span>]</p> <input type="range" min="0" max="100" value="50" id="gainSlider"/> <p>Invert Phase [<span id="invertPhaseValueLabel"></span>]</p> <input type="checkbox" id="toggleInvertPhase"/>'},m(n,r){R(n,t,r)},p:h,i:h,o:h,d(n){n&&N(t)}}}function fe(e,t,n,r,a){return(Math.min(Math.max(e,t),n)-t)/(n-t)*(a-r)+r}function ge(){console.log("setupParameterController");const e=document.getElementById("gainSlider"),t=document.getElementById("gainValueLabel"),n=document.getElementById("toggleInvertPhase"),r=document.getElementById("invertPhaseValueLabel");e.addEventListener("input",function(){const a={sliderName:e.id,sliderValue:e.value,sliderRangeMin:e.min,sliderRangeMax:e.max};t.innerText=e.value,window.onSliderChanged(a)}),n.addEventListener("change",function(){const a={toggleName:n.id,toggleValue:n.checked};n.checked?r.innerText="ON":r.innerText="OFF",window.onToggleChanged(a)})}function me(e){console.log("JSON from C++: onParameterChanged - ",e);const t=e.parameterName;if(t==="gain"){const n=document.getElementById("gainSlider"),r=document.getElementById("gainValueLabel"),a=fe(e.parameterValue,0,1,n.min,n.max);console.log("gainSlider: ",a),n.value=a,r.innerText=n.value}else if(t==="invertPhase"){const n=document.getElementById("toggleInvertPhase"),r=document.getElementById("invertPhaseValueLabel");console.log("invertPhase: ",e.parameterValue),n.checked=e.parameterValue,n.checked?r.innerHTML="ON":r.innerText="OFF"}}class he extends U{constructor(t){super(),G(this,t,null,de,A,{})}}function pe(e){let t,n,r,a,o,s,p,f,i,$,l,u,g;return u=new he({}),{c(){t=m("main"),n=m("h1"),n.textContent="DenoGain",r=L(),a=m("img"),s=L(),p=m("div"),p.innerHTML=`<a href="https://vitejs.dev" target="_blank" rel="noreferrer"><img src="/vite.svg" class="logo svelte-10mevpo" alt="Vite Logo"/></a> <a href="https://svelte.dev" target="_blank" rel="noreferrer"><img src="${ue}" class="logo svelte svelte-10mevpo" alt="Svelte Logo"/></a>`,f=L(),i=m("h1"),i.textContent="Vite + Svelte",$=L(),l=m("div"),le(u.$$.fragment),D(a.src,o="/vite-deno.svg")||P(a,"src",o),P(a,"alt","Vite with Deno"),P(a,"class","logo svelte-10mevpo"),P(l,"class","card")},m(c,J){R(c,t,J),d(t,n),d(t,r),d(t,a),d(t,s),d(t,p),d(t,f),d(t,i),d(t,$),d(t,l),z(u,l,null),g=!0},p:h,i(c){g||(q(u.$$.fragment,c),g=!0)},o(c){ie(u.$$.fragment,c),g=!1},d(c){c&&N(t),H(u)}}}function _e(e){return ee(()=>{ge(),window.onParameterChanged=me,window.onInitialUpdate()}),[]}class ve extends U{constructor(t){super(),G(this,t,_e,pe,A,{})}}new ve({target:document.getElementById("app")});