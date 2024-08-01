//robot.js
var tempData = [
  30, 32, 31, 33, 34, 35, 36, 37, 35, 34, 32, 30, 31, 29, 28, 27, 26, 25, 24,
  23,
];
var humidityData = [
  50, 52, 51, 53, 54, 55, 56, 57, 55, 54, 52, 50, 51, 49, 48, 47, 46, 45, 44,
  43,
];
var labels = Array.from({ length: 20 }, (_, i) =>
  new Date(Date.now() - (19 - i) * 1000 * 60).toLocaleTimeString()
);

// Function to add data points to arrays
function addDataPoint(temp, humidity) {
  var now = new Date().toLocaleTimeString();
  labels.push(now);
  tempData.push(temp);
  humidityData.push(humidity);

  // Keep only the last 20 records
  if (labels.length > 20) {
    labels.shift();
    tempData.shift();
    humidityData.shift();
  }
}

// Function to fetch data from JSON and update the charts
async function fetchData() {
  try {
    let response = await fetch("http://localhost:8000/data1.json");
    if (response.ok) {
      let jsonData = await response.json();
      var temp = jsonData.Temp;
      var humidity = jsonData.Humidity;
      addDataPoint(temp, humidity);

      // Update the charts with new data
      tempChart.data.labels = labels;
      tempChart.data.datasets[0].data = tempData;
      tempChart.update();

      humidityChart.data.labels = labels;
      humidityChart.data.datasets[0].data = humidityData;
      humidityChart.update();

      // Update the current values below the charts
      document.getElementById(
        "tempValue"
      ).textContent = `Current Temperature: ${temp}°C`;
      document.getElementById(
        "humidityValue"
      ).textContent = `Current Humidity: ${humidity}%`;
    } else {
      console.error("Network response was not ok.");
    }
  } catch (error) {
    console.error("Fetch error:", error);
  }
}

// Initialize charts
const tempCtx = document.getElementById("tempChart").getContext("2d");
const tempChart = new Chart(tempCtx, {
  type: "line",
  data: {
    labels: labels,
    datasets: [
      {
        label: "Temperature",
        backgroundColor: "rgba(255, 99, 132, 0.2)",
        borderColor: "rgba(255, 99, 132, 1)",
        borderWidth: 1,
        data: tempData,
        fill: false,
      },
    ],
  },
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        min: 0,
      },
    },
  },
});

const humidityCtx = document.getElementById("humidityChart").getContext("2d");
const humidityChart = new Chart(humidityCtx, {
  type: "line",
  data: {
    labels: labels,
    datasets: [
      {
        label: "Humidity",
        backgroundColor: "rgba(54, 162, 235, 0.2)",
        borderColor: "rgba(54, 162, 235, 1)",
        borderWidth: 1,
        data: humidityData,
        fill: false,
      },
    ],
  },
  options: {
    maintainAspectRatio: false,
    scales: {
      y: {
        beginAtZero: true,
        min: 0,
      },
    },
  },
});

// Fetch data initially and then every 2 seconds
fetchData();
setInterval(fetchData, 2000);
