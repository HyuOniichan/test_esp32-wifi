// config_wifi
// Send Wifi config
function connectWiFi() {
	const mode = document.getElementById("selectMode").value;
	const ssid = document.getElementById("ssidInput").value;
	const pass = document.getElementById("passInput").value;
	const mac = document.getElementById("macInput").value;

	alert(
		"Mode: " + mode 
		+ "\nSSID: " + ssid 
		+ "\nPassword: " + pass
		+ "\nMAC Address: " + mac
	);

	// Send to ESP32 WebServer
	fetch("/api/wifi", {
		method: "POST",
		headers: { "Content-Type": "application/json" },
		body: JSON.stringify({ mode, ssid, pass, mac }),
	})
		.then((res) => res.json())
		.then((data) => {
			alert(data.status || "OK");
		});
}

// index.html
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

// upload_ota
// Send source code OTA to ESP
function uploadFirmware() {
    let file = document.getElementById("otaFile").files[0];
    let formData = new FormData();
    formData.append("file", file);

	// Maybe better for multipart upload
    let xhr = new XMLHttpRequest();

    xhr.open("POST", "/api/ota", true);

    // progress
    xhr.upload.onprogress = function(e) {
        if (e.lengthComputable) {
            let percent = (e.loaded / e.total) * 100;
            document.getElementById("progress").value = percent;
        }
    };

    xhr.onload = function() {
        if (xhr.status == 200) {
            alert("OTA success! Rebooting...");
        } else {
            alert("OTA failed");
        }
    };

    xhr.send(formData);
}


// Auto load on page open
refreshData();
