console.out("Sending troggle drive.");

include("scripts/CheckStates.js");

checkSend(["PWR_CONTROL"]);
dim.send("PWR_CONTROL/TOGGLE_DRIVE");
