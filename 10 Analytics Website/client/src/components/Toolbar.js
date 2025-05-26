import { useState } from "react";
import "./styles/Toolbar.css"

export default function Toolbar({options}) {
    buttons = options.map((options) => <button onClick={options.onClick}>{options.name}</button>);
    
}