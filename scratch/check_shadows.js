const fs = require('fs');
const content = fs.readFileSync('db/re/item_db_equip.yml', 'utf8');
const ids = [24001, 24002, 24004, 24005, 24666, 24667];
for (const id of ids) {
  const reg = new RegExp(`-\\s*Id:\\s*${id}[\\s\\S]*?Name:\\s*([^\\r\\n]+)`, 'g');
  const m = reg.exec(content);
  if (m) console.log(id, m[1]);
}
