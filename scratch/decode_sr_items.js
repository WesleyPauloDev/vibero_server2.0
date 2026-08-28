const fs = require('fs');

const items = [
  { equip: 1, nameid: 420110, refine: 0, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Head_Low / Baixo" },
  { equip: 2, nameid: 26140, refine: 12, card0: 27384, card1: 27384, card2: 8009, card3: 4719, slot: "Right_Hand / Arma" },
  { equip: 4, nameid: 480188, refine: 12, card0: 300521, card1: 8109, card2: 8009, card3: 4719, slot: "Garment / Capa" },
  { equip: 8, nameid: 32258, refine: 0, card0: 300369, card1: 8109, card2: 8009, card3: 4719, slot: "Accessory_Left / Acessório Esq." },
  { equip: 16, nameid: 450206, refine: 12, card0: 300254, card1: 311249, card2: 311179, card3: 311098, slot: "Armor / Armadura" },
  { equip: 32, nameid: 460020, refine: 12, card0: 27385, card1: 8109, card2: 8009, card3: 4719, slot: "Left_Hand / Escudo" },
  { equip: 64, nameid: 470112, refine: 12, card0: 4578, card1: 8109, card2: 8009, card3: 4719, slot: "Shoes / Calçados" },
  { equip: 128, nameid: 490068, refine: 0, card0: 300369, card1: 8129, card2: 8009, card3: 4719, slot: "Accessory_Right / Acessório Dir." },
  { equip: 256, nameid: 19487, refine: 12, card0: 300258, card1: 29080, card2: 4714, card3: 4714, slot: "Head_Top / Topo" },
  { equip: 512, nameid: 410026, refine: 0, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Head_Mid / Meio" },
  { equip: 1024, nameid: 19961, refine: 0, card0: 29651, card1: 0, card2: 0, card3: 0, slot: "Costume Head_Top" },
  { equip: 2048, nameid: 5911, refine: 0, card0: 29652, card1: 0, card2: 0, card3: 0, slot: "Costume Head_Mid" },
  { equip: 4096, nameid: 5977, refine: 0, card0: 29653, card1: 0, card2: 0, card3: 0, slot: "Costume Head_Low" },
  { equip: 8192, nameid: 20761, refine: 0, card0: 29654, card1: 310662, card2: 0, card3: 0, slot: "Costume Garment" },
  { equip: 65536, nameid: 24667, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Weapon" },
  { equip: 131072, nameid: 24001, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Armor" },
  { equip: 262144, nameid: 24002, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Shield" },
  { equip: 524288, nameid: 24666, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Shoes" },
  { equip: 1048576, nameid: 24004, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Earring" },
  { equip: 2097152, nameid: 24005, refine: 10, card0: 0, card1: 0, card2: 0, card3: 0, slot: "Shadow Pendant" },
];

const allIds = new Set();
items.forEach(it => {
  allIds.add(it.nameid);
  if (it.card0) allIds.add(it.card0);
  if (it.card1) allIds.add(it.card1);
  if (it.card2) allIds.add(it.card2);
  if (it.card3) allIds.add(it.card3);
});

// Search in item dbs
const dbFiles = [
  'db/re/item_db_equip.yml',
  'db/re/item_db_etc.yml',
  'db/re/item_db_custom.yml',
  'db/re/item_db.yml'
];

const nameMap = {};

for (const file of dbFiles) {
  if (!fs.existsSync(file)) continue;
  const content = fs.readFileSync(file, 'utf8');
  for (const id of allIds) {
    const regex = new RegExp(`-\\s*Id:\\s*${id}\\r?\\n\\s*AegisName:\\s*([^\\r\\n]+)\\r?\\n\\s*Name:\\s*([^\\r\\n]+)`, 'g');
    let m = regex.exec(content);
    if (m) {
      nameMap[id] = { aegis: m[1].trim(), name: m[2].trim() };
    }
  }
}

console.log("Found names count:", Object.keys(nameMap).length);
items.forEach(it => {
  const itemInfo = nameMap[it.nameid] || { name: `Unknown (${it.nameid})` };
  const c0 = nameMap[it.card0] ? nameMap[it.card0].name : (it.card0 ? `Card/Enchant ${it.card0}` : 'None');
  const c1 = nameMap[it.card1] ? nameMap[it.card1].name : (it.card1 ? `Card/Enchant ${it.card1}` : 'None');
  const c2 = nameMap[it.card2] ? nameMap[it.card2].name : (it.card2 ? `Card/Enchant ${it.card2}` : 'None');
  const c3 = nameMap[it.card3] ? nameMap[it.card3].name : (it.card3 ? `Card/Enchant ${it.card3}` : 'None');
  console.log(`[${it.slot}] +${it.refine} ${itemInfo.name} (${it.nameid}) | C0: ${c0} | C1: ${c1} | C2: ${c2} | C3: ${c3}`);
});
