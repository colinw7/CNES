#ifndef CQNESDbg_H
#define CQNESDbg_H

#include <CQ6502Dbg.h>
#include <CQ6502Memory.h>

class CQNESPPUMem;
class CQ6502HexEdit;

namespace CNES {

class QMachine;

}

class CQNESDbg : public CQ6502Dbg {
  Q_OBJECT

 public:
  CQNESDbg(CNES::QMachine *machine);

  void setFixedFont(const QFont &font) override;

  void addLeftWidgets () override;
  void addRightWidgets() override;

  void setTraced(bool traced) override;

 private slots:
  void nesChrLSlot(unsigned int page);
  void nesChrHSlot(unsigned int page);

  void nesDebugReadSlot (int state);
  void nesDebugWriteSlot(int state);

  void ppuDebugReadSlot (int state);
  void ppuDebugWriteSlot(int state);
  void ppuScanLineSlot  (int state);

  void cartDebugSlot(int state);

  void updateNESSlot();

  void updatePPUSlot(ushort addr, ushort len);

  void ppuMemEnabledSlot();

  void illegalJumpSlot();

  void nmiSlot();

 private:
  CNES::QMachine *machine_ { nullptr };

  struct NESData {
    QGroupBox*     group                 { nullptr };

    QCheckBox*     debugReadCheck        { nullptr };
    QCheckBox*     debugWriteCheck       { nullptr };

    QCheckBox*     illegalJumpCheck      { nullptr };
    QCheckBox*     nmiCheck              { nullptr };

    CQ6502HexEdit* nameTableEdit         { nullptr };
    QCheckBox*     verticalWriteCheck    { nullptr };

    QCheckBox*     screenVisibleCheck    { nullptr };
    CQ6502HexEdit* screenPatternAddrEdit { nullptr };

    QCheckBox*     spritesVisibleCheck   { nullptr };
    CQ6502HexEdit* spritePatternAddrEdit { nullptr };
    CQ6502HexEdit* spriteSizeEdit        { nullptr };
  //QCheckBox*     hitInterruptCheck     { nullptr };

    QCheckBox*     imageMaskCheck        { nullptr };
    QCheckBox*     spriteMaskCheck       { nullptr };

    QCheckBox*     blankInterruptCheck   { nullptr };

    CQ6502HexEdit* scrollVEdit           { nullptr };
    CQ6502HexEdit* scrollHEdit           { nullptr };
  };

  struct PPUData {
    QGroupBox* group           { nullptr };
    QCheckBox* debugReadCheck  { nullptr };
    QCheckBox* debugWriteCheck { nullptr };
    QCheckBox* scanLineCheck   { nullptr };
  };

  struct CartData {
    QGroupBox*     group          { nullptr };
    QCheckBox*     debugCheck     { nullptr };
    CQ6502HexEdit* prgSizeEdit    { nullptr };
    CQ6502HexEdit* chrSizeEdit    { nullptr };
    QCheckBox*     mirroringCheck { nullptr };
    CQ6502HexEdit* prgRamSizeEdit { nullptr };
    CQ6502HexEdit* mapperEdit     { nullptr };

    CQ6502HexEdit* chrLEdit       { nullptr };
    CQ6502HexEdit* chrHEdit       { nullptr };
  };

  struct PPUMemData {
    QGroupBox*   group   { nullptr };
    CQNESPPUMem* memArea { nullptr };
  };

  NESData    nesData_;
  PPUData    ppuData_;
  CartData   cartData_;
  PPUMemData ppuMemData_;
};

//---

class CQNESPPUMem : public CQMemArea {
  Q_OBJECT

 public:
  CQNESPPUMem(CNES::QMachine *machine);

  //---

  uint memorySize() const override { return 16384; }

  uint getCurrentAddress() const override;
  void setCurrentAddress(uint addr) override;

  bool isReadOnlyAddr(uint addr, uint len) const override;
  bool isScreenAddr(uint addr, uint len) const override;

  uchar getByte(uint addr) const override;

 private:
  CNES::QMachine *machine_ { nullptr };
};

#endif
