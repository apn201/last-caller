#pragma once
// purse.h: the shared purse (gold and bank) and the Legend record it grew from. The Legend
// and dice doors are gone from the door table and the code; this keeps the files
// VolaMUD and the SysOp's user editor still read and write (DATA/DOORS/WALLET.DAT,
// KOUVQUEST.DAT). Depends on core/session.h (Quest, Wallet) and core/storage.
// Included before doors/volamud.h and bbs/user_editor.h.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static constexpr uint8_t MAX_GEAR = 3;
static bool parseQuest(const String &line, Quest &q) {
  q = Quest();
  q.id = strtoul(field(line, 0).c_str(), nullptr, 10);
  q.level = constrain(field(line, 1).toInt(), 1L, 12L);
  q.maxHp = constrain(field(line, 3).toInt(), 1L, 65535L);
  q.hp = constrain(field(line, 2).toInt(), 1L, long(q.maxHp));
  q.gold = strtoul(field(line, 4).c_str(), nullptr, 10);
  q.bank = strtoul(field(line, 5).c_str(), nullptr, 10);
  q.exp = strtoul(field(line, 6).c_str(), nullptr, 10);
  q.fights = constrain(field(line, 7).toInt(), 0L, 255L);
  q.weapon = constrain(field(line, 8).toInt(), 0L, long(MAX_GEAR));
  q.armor = constrain(field(line, 9).toInt(), 0L, long(MAX_GEAR));
  q.kills = strtoul(field(line, 10).c_str(), nullptr, 10);
  q.pvpWins = strtoul(field(line, 11).c_str(), nullptr, 10);
  q.pvpLosses = strtoul(field(line, 12).c_str(), nullptr, 10);
  q.fame = strtoul(field(line, 13).c_str(), nullptr, 10);
  q.day = strtoul(field(line, 14).c_str(), nullptr, 10);
  return q.id > 0;
}
static String questLine(const Quest &q) {
  return String(q.id) + "|" + q.level + "|" + q.hp + "|" + q.maxHp + "|" +
         q.gold + "|" + q.bank + "|" + q.exp + "|" + q.fights + "|" + q.weapon + "|" + q.armor +
         "|" + q.kills + "|" + q.pvpWins + "|" + q.pvpLosses + "|" + q.fame + "|" + q.day;
}
static constexpr const char *QUEST_DATA = "/BBS/DATA/DOORS/KOUVQUEST.DAT";
static constexpr const char *WALLET_DATA = "/BBS/DATA/DOORS/WALLET.DAT";
static bool parseWallet(const String &line, Wallet &wallet) {
  wallet = Wallet();
  wallet.id = strtoul(field(line, 0).c_str(), nullptr, 10);
  wallet.gold = strtoul(field(line, 1).c_str(), nullptr, 10);
  wallet.bank = strtoul(field(line, 2).c_str(), nullptr, 10);
  return wallet.id > 0;
}
static String walletLine(const Wallet &wallet) {
  return String(wallet.id) + "|" + wallet.gold + "|" + wallet.bank + "|V1";
}
static bool saveSharedWallet(const Wallet &wallet) { return putRecord(WALLET_DATA, wallet.id, walletLine(wallet)); }
static bool loadSharedWallet(uint32_t id, Wallet &wallet) {
  wallet = Wallet(); wallet.id = id;
  if (!sdReady || !id || !recoverFile(WALLET_DATA)) return false;
  File file = SD_MMC.open(WALLET_DATA, FILE_READ);
  if (file) {
    Wallet candidate;
    while (file.available()) {
      if (parseWallet(file.readStringUntil('\n'), candidate) && candidate.id == id) {
        wallet = candidate;
        file.close();
        return true;
      }
    }
    file.close();
  } else if (SD_MMC.exists(WALLET_DATA)) return false;
  // One-time migration: existing Legend purses become the shared
  // purse when a caller first reaches any of the three doors.
  File questFile = SD_MMC.open(QUEST_DATA, FILE_READ);
  if (questFile) {
    Quest old;
    while (questFile.available()) {
      if (parseQuest(questFile.readStringUntil('\n'), old) && old.id == id) {
        wallet.gold = old.gold; wallet.bank = old.bank;
        break;
      }
    }
    questFile.close();
  }
  return saveSharedWallet(wallet);
}
static bool loadQuest(uint32_t id, Quest &quest) {
  quest = Quest();
  quest.id = id;
  if (!sdReady || !recoverFile(QUEST_DATA)) return false;
  File file = SD_MMC.open(QUEST_DATA, FILE_READ);
  if (!file && SD_MMC.exists(QUEST_DATA)) return false;
  Quest candidate;
  while (file && file.available()) {
    if (parseQuest(file.readStringUntil('\n'), candidate) && candidate.id == id) {
      quest = candidate;
      break;
    }
  }
  if (file) file.close();
  Wallet wallet;
  if (!loadSharedWallet(id, wallet)) return false;
  quest.gold = wallet.gold;
  quest.bank = wallet.bank;
  return true;
}
static bool saveQuest(const Quest &quest) {
  Wallet wallet;  // gold and bank live in the purse the doors share
  wallet.id = quest.id; wallet.gold = quest.gold; wallet.bank = quest.bank;
  return saveSharedWallet(wallet) && putRecord(QUEST_DATA, quest.id, questLine(quest));
}
