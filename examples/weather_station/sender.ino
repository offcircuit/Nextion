String decimate(byte b) {
  return ((b < 10) ? "0" : "") + String(b);
}

void sendData() {
  String m[12] = {"Janeiro", "Fevereiro", "Mar" + String(char(0xE7)) + "o", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
  String w[7] = {"Domingo", "Segunda", "Ter" + String(char(0xE7)) + "a", "Quarta", "Quinta", "Sexta", "S" + String(char(0xE1)) + "bado"};

  sendText(w[dtime.weekday] + ", " + String(dtime.day) + " " + m[dtime.month - 1] + " " + String(dtime.year), {0, 2});
  sendText(decimate(dtime.hour) + ":" + decimate(dtime.minute) + ":" + decimate(dtime.second), {0, 1});

  if (dht.read(DHT22, 2)) {
    sendText(String(dht.celsius / 10, 0) + String(char(0xB0)) + "c", {0, 7});
    sendText(String(dht.humidity / 10, 1) + "%", {0, 6});
  }
}

void sendFormat(String s) {
  if (event) sendText(s.substring(0, 2) + "-" + s.substring(2, 4) + "-" + s.substring(4), {2, 13});
  else sendText(s.substring(0, 2) + ":" + s.substring(2, 4) + ":" + s.substring(4), {2, 13});
}

void sendText(String s, nextionComponent c) {
  nextion.print("p[" + String(c.page) + "].b[" + String(c.id) + "].txt=\"" + String(s) + "\"");
}
