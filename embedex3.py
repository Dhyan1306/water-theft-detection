from flask import Flask, request, render_template_string
import sqlite3
from datetime import datetime
app = Flask(__name__)
def init_db():
    conn = sqlite3.connect("water.db")
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS readings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            inlet REAL,
            outlet REAL,
            difference REAL,
            status TEXT,
            timestamp TEXT
        )
    """)
    conn.commit()
    conn.close()

init_db()

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json

    inlet = float(data['inlet'])
    outlet = float(data['outlet'])
    difference = inlet - outlet

    
    if difference > 1:
        status = "THEFT DETECTED"
    else:
        status = "NORMAL"

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    conn = sqlite3.connect("water.db")
    cursor = conn.cursor()
    cursor.execute("""
        INSERT INTO readings (inlet, outlet, difference, status, timestamp)
        VALUES (?, ?, ?, ?, ?)
    """, (inlet, outlet, difference, status, timestamp))
    conn.commit()
    conn.close()

    return {"message": "Data saved successfully"}, 200



@app.route('/')
def dashboard():
    conn = sqlite3.connect("water.db")
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM readings ORDER BY id DESC LIMIT 1")
    row = cursor.fetchone()

   
    cursor.execute("SELECT inlet, outlet, timestamp FROM readings ORDER BY id DESC LIMIT 10")
    rows = cursor.fetchall()
    conn.close()

    if row:
        inlet, outlet, diff, status = row[1], row[2], row[3], row[4]
    else:
        inlet = outlet = diff = 0
        status = "No Data"

    rows.reverse()

    inlet_data = [r[0] for r in rows]
    outlet_data = [r[1] for r in rows]
    labels = [r[2][-8:] for r in rows]

    html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Water Theft Monitoring</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <meta http-equiv="refresh" content="5">
</head>

<body class="bg-dark text-light">

<div class="container mt-5">

    <h1 class="text-center mb-4">Smart Water Theft Monitoring</h1>

    <div class="row text-center mb-4">

        <div class="col-md-4">
            <div class="card bg-primary p-3 rounded-4 shadow">
                <h5>Inlet Flow</h5>
                <h2>{inlet} L/min</h2>
            </div>
        </div>

        <div class="col-md-4">
            <div class="card bg-info p-3 rounded-4 shadow">
                <h5>Outlet Flow</h5>
                <h2>{outlet} L/min</h2>
            </div>
        </div>
        <div class="col-md-3">
        <div class="card bg-warning p-3 rounded-4 shadow">
            <h5>Difference</h5>
            <h2>{diff} L/min</h2>
        </div>
    </div>

        <div class="col-md-4">
            <div class="card {"bg-danger" if status=="THEFT DETECTED" else "bg-success"} p-3 rounded-4 shadow">
                <h5>Status</h5>
                <h2>{status}</h2>
            </div>
        </div>

    </div>

    <div class="card bg-secondary p-4 rounded-4 shadow">
        <h4 class="text-center mb-3">Flow Trend (Last 10 Readings)</h4>
        <canvas id="flowChart"></canvas>
    </div>

</div>

<script>
const ctx = document.getElementById('flowChart');

new Chart(ctx, {{
    type: 'line',
    data: {{
        labels: {labels},
        datasets: [
            {{
                label: 'Inlet Flow',
                data: {inlet_data},
                borderColor: 'cyan',
                tension: 0.3
            }},
            {{
                label: 'Outlet Flow',
                data: {outlet_data},
                borderColor: 'yellow',
                tension: 0.3
            }}
        ]
    }},
    options: {{
        responsive: true,
        plugins: {{
            legend: {{
                labels: {{
                    color: 'white'
                }}
            }}
        }},
        scales: {{
            x: {{
                ticks: {{ color: 'white' }}
            }},
            y: {{
                ticks: {{ color: 'white' }}
            }}
        }}
    }}
}});
</script>

</body>
</html>
"""
    return render_template_string(html)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)