function connectWiFi() {
	const mode = document.getElementById("selectMode").value;
	const ssid = document.getElementById("ssidInput").value;
	const pass = document.getElementById("passInput").value;

	alert("Mode: " + mode + "\nSSID: " + ssid + "\nPassword: " + pass);

	// Send to ESP32 WebServer
	fetch("/api/wifi", {
		method: "POST",
		headers: { "Content-Type": "application/json" },
		body: JSON.stringify({ mode, ssid, pass }),
	})
		.then((res) => res.json())
		.then((data) => {
			alert(data.status || "OK");
		});
}

// Fetch WiFi config from ESP32
function refreshData() {
	fetch("/api/wifi")
		.then((res) => res.json())
		.then((data) => {
			document.getElementById("ssidText").innerText = data.ssid || "N/A";
			document.getElementById("modeText").innerText = data.mode || "N/A";
			document.getElementById("ipText").innerText = data.ip || "N/A";
		})
		.catch((err) => {
			document.getElementById("ssidText").innerText = "Error";
			document.getElementById("modeText").innerText = "Error";
			document.getElementById("ipText").innerText = "Error";
		});
}

// Auto load on page open
refreshData();
