#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

#include "hafiza_manager.h"
#include "dhtsensor.h"
#include "susensor.h"
#include "toprak_sensor.h"

const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Garden PRO</title>
  <style>
    :root { --bg: #0f172a; --card: #1e293b; --accent: #22c55e; --text: #f8fafc; --danger: #ef4444; }
    body { font-family: 'Segoe UI', sans-serif; background: var(--bg); color: var(--text); margin: 0; padding: 10px; text-align: center; }
    .header { padding: 15px; background: var(--card); border-radius: 12px; margin-bottom: 15px; border-bottom: 3px solid var(--accent); }
    
    .grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; margin-bottom: 15px; }
    @media (min-width: 600px) { .grid { grid-template-columns: repeat(3, 1fr); } }
    
    .card { background: var(--card); padding: 15px; border-radius: 12px; border: 1px solid #334155; position: relative; overflow: hidden; }
    .card h3 { font-size: 10px; color: #94a3b8; margin: 0 0 5px 0; text-transform: uppercase; letter-spacing: 1px; }
    .value { font-size: 22px; font-weight: bold; color: var(--accent); }
    .icon { font-size: 20px; margin-bottom: 5px; display: block; }
    
    .analysis { background: var(--card); padding: 15px; border-radius: 12px; margin-bottom: 15px; text-align: left; }
    .analysis h2 { font-size: 16px; color: var(--accent); margin: 0 0 15px 0; display: flex; align-items: center; gap: 8px; }
    .bar-container { margin-bottom: 12px; }
    .bar-label { display: flex; justify-content: space-between; font-size: 13px; margin-bottom: 4px; color: #94a3b8; font-weight: 500; }
    .bar-bg { background: #334155; height: 10px; border-radius: 5px; overflow: hidden; }
    .bar-fill { height: 100%; width: 0%; background: var(--accent); transition: width 0.6s cubic-bezier(0.4, 0, 0.2, 1); }

    .settings { background: var(--card); padding: 15px; border-radius: 12px; text-align: left; }
    .settings h2 { font-size: 16px; margin: 0 0 15px 0; color: var(--accent); border-bottom: 1px solid #334155; padding-bottom: 8px; display: flex; align-items: center; gap: 8px; }
    .input-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px; background: #334155; padding: 10px; border-radius: 8px; }
    .input-row label { font-size: 13px; display: flex; align-items: center; gap: 5px; }
    input { background: var(--bg); border: 1px solid #475569; color: white; padding: 6px; border-radius: 4px; width: 55px; text-align: center; font-weight: bold; }
    .btn-save { background: var(--accent); color: white; border: none; padding: 15px; width: 100%; border-radius: 10px; font-size: 16px; font-weight: bold; cursor: pointer; margin-top: 10px; display: flex; justify-content: center; align-items: center; gap: 10px; }
    .btn-save:hover { opacity: 0.9; }
    .footer { margin-top: 15px; padding-bottom: 20px; }
    .footer a { color: #94a3b8; text-decoration: none; font-size: 13px; display: flex; align-items: center; justify-content: center; gap: 5px; }
  </style>
</head>
<body>
  <div class="header">
    <h1 style="margin:0; font-size: 18px;">🌱 SMART GARDEN PRO</h1>
    <div id="status" style="font-size:10px; color:var(--accent);">● SİSTEM ÇEVRİMİÇİ</div>
  </div>

  <div class="grid">
    <div class="card"><span class="icon">🧪</span><h3>PH DEĞERİ</h3><div class="value" id="v_ph">--</div></div>
    <div class="card"><span class="icon">⚡</span><h3>EC DEĞERİ</h3><div class="value" id="v_ec">--</div></div>
    <div class="card"><span class="icon">🌡️</span><h3>ORTAM ISISI</h3><div class="value"><span id="v_temp">--</span>°C</div></div>
    <div class="card"><span class="icon">☁️</span><h3>ORTAM NEMİ</h3><div class="value">%<span id="v_hum">--</span></div></div>
    <div class="card"><span class="icon">💧</span><h3>SU ISISI</h3><div class="value"><span id="v_wtemp">--</span>°C</div></div>
    <div class="card"><span class="icon">🪴</span><h3>TOPRAK NEMİ</h3><div class="value">%<span id="v_soil">--</span></div></div>
  </div>

  <div class="analysis">
    <h2>📊 SENSÖR ANALİZİ</h2>
    <div id="soil_bars"></div>
  </div>

  <div class="settings">
    <h2>⚙️ SİSTEM AYARLARI</h2>
    <form action="/save_limits" method="POST">
      <div class="input-row"><label>🌡️ Isı (Min/Max):</label> <div><input name="imin" id="imin"> <input name="imax" id="imax"></div></div>
      <div class="input-row"><label>☁️ Nem (Min/Max):</label> <div><input name="nmin" id="nmin"> <input name="nmax" id="nmax"></div></div>
      <div class="input-row"><label>🧪 pH (Min/Max):</label> <div><input name="pmin" id="pmin"> <input name="pmax" id="pmax"></div></div>
      <div class="input-row"><label>⚡ EC (Min/Max):</label> <div><input name="emin" id="emin"> <input name="emax" id="emax"></div></div>
      <div class="input-row"><label>🪴 Toprak (Min/Max):</label> <div><input name="tmin" id="tmin"> <input name="tmax" id="tmax"></div></div>
      <button type="submit" class="btn-save">💾 AYARLARI KAYDET</button>
    </form>
  </div>

  <div class="footer"><a href="/">⬅️ Ana Giriş Kapısına Dön</a></div>

  <script>
    const container = document.getElementById('soil_bars');
    for(let i=1; i<=6; i++) {
      container.innerHTML += `
        <div class="bar-container">
          <div class="bar-label"><span>Sensör ${i} Analizi</span><span id="s${i}_val">%0</span></div>
          <div class="bar-bg"><div id="s${i}_bar" class="bar-fill"></div></div>
        </div>`;
    }

    function updateData() {
      fetch('/read_sensors').then(r => r.json()).then(data => {
        document.getElementById("v_temp").innerText = data.temp;
        document.getElementById("v_hum").innerText = data.hum;
        document.getElementById("v_wtemp").innerText = data.wtemp;
        document.getElementById("v_soil").innerText = data.soil;
        document.getElementById("v_ph").innerText = "6.2"; 
        document.getElementById("v_ec").innerText = "1.4";

        if(document.getElementById("imin").value == "") {
           document.getElementById("imin").value = data.lim.imin; document.getElementById("imax").value = data.lim.imax;
           document.getElementById("nmin").value = data.lim.nmin; document.getElementById("nmax").value = data.lim.nmax;
           document.getElementById("pmin").value = data.lim.pmin; document.getElementById("pmax").value = data.lim.pmax;
           document.getElementById("emin").value = data.lim.emin; document.getElementById("emax").value = data.lim.emax;
           document.getElementById("tmin").value = data.lim.tmin; document.getElementById("tmax").value = data.lim.tmax;
        }

        for(let i=0; i<6; i++) {
          let val = data.saksilar[i];
          document.getElementById(`s${i+1}_val`).innerText = '%' + val;
          let bar = document.getElementById(`s${i+1}_bar`);
          bar.style.width = val + '%';
          bar.style.background = val < 25 ? '#ef4444' : (val < 60 ? '#f59e0b' : '#22c55e');
        }
        document.getElementById("status").style.color = "#22c55e";
        document.getElementById("status").innerText = "● SİSTEM ÇEVRİMİÇİ";
      }).catch(e => {
        document.getElementById("status").innerText = "○ BAĞLANTI KESİLDİ";
        document.getElementById("status").style.color = "#ef4444";
      });
    }
    setInterval(updateData, 2000);
    updateData();
  </script>
</body>
</html>
)rawliteral";

#endif