async function fetchAndCreateChart() {
  try {
    const response = await fetch("data.json");
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    const jsonData = await response.json();

    console.log("Fetched JSON Data:", jsonData);

    const ctx = document.getElementById("myChart").getContext("2d");

    new Chart(ctx, {
      type: "line",
      data: {
        labels: ["0", "Current"],
        datasets: [
          {
            label: "Temp",
            backgroundColor: "rgba(255, 99, 132, 0.2)",
            borderColor: "rgba(255, 99, 132, 1)",
            borderWidth: 1,
            data: [0, jsonData.Temp],
            fill: false,
          },
          {
            label: "Humidity",
            backgroundColor: "rgba(54, 162, 235, 0.2)",
            borderColor: "rgba(54, 162, 235, 1)",
            borderWidth: 1,
            data: [0, jsonData.Humidity],
            fill: false,
          },
          {
            label: "Angle",
            backgroundColor: "rgba(75, 192, 192, 0.2)",
            borderColor: "rgba(75, 192, 192, 1)",
            borderWidth: 1,
            data: [0, jsonData.angle],
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
  } catch (error) {
    console.error("Error fetching JSON data:", error);
  }
}

fetchAndCreateChart();
