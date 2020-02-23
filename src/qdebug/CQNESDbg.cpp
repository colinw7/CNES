#include <CQNESDbg.h>

#include <CQNES_Machine.h>
#include <CQNES_CPU.h>
#include <CQNES_PPU.h>
#include <CQNES_Cartridge.h>

#include <CQ6502HexEdit.h>

#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>

using namespace CNES;

CQNESDbg::
CQNESDbg(QMachine *machine) :
 machine_(machine)
{
  setCPU(machine_->getCPU());

//connect(machine, SIGNAL(registerChangedSignal()), this, SLOT(updateSlot()));

//connect(machine, SIGNAL(flagsChangedSignal()), this, SLOT(updateSlot()));
//connect(machine, SIGNAL(stackChangedSignal()), this, SLOT(updateSlot()));

  connect(machine, SIGNAL(pcChangedSignal()), this, SLOT(updateSlot()));

//connect(machine, SIGNAL(memChangedSignal(ushort addr, ushort len)), this, SLOT(updateSlot()));

  connect(machine, SIGNAL(handleBreakSignal()), this, SLOT(forceHalt()));
  connect(machine, SIGNAL(breakpointHitSignal()), this, SLOT(forceHalt()));
  connect(machine, SIGNAL(illegalJumpSignal()), this, SLOT(illegalJumpSlot()));
  connect(machine, SIGNAL(nmiSignal()), this, SLOT(nmiSlot()));

  connect(machine, SIGNAL(breakpointsChangedSignal()), this, SLOT(breakpointsChangedSlot()));

  connect(machine->getQCPU(), SIGNAL(nesChangedSignal()), this, SLOT(updateNESSlot()));

  connect(machine->getQPPU(), SIGNAL(memChangedSignal(ushort, ushort)),
          this, SLOT(updatePPUSlot(ushort, ushort)));
}

void
CQNESDbg::
addLeftWidgets()
{
  CQ6502Dbg::addLeftWidgets();

  //---

  ppuMemData_.group = CQUtil::makeLabelWidget<QGroupBox>("Enabled", "ppuGroup");
  ppuMemData_.group->setCheckable(true);

  connect(ppuMemData_.group, SIGNAL(toggled(bool)), this, SLOT(ppuMemEnabledSlot()));

  auto ppuGroupLayout = CQUtil::makeLayout<QVBoxLayout>(ppuMemData_.group, 2, 2);

  ppuMemData_.memArea = new CQNESPPUMem(machine_);

  ppuGroupLayout->addWidget(ppuMemData_.memArea);

  addLeftWidget(ppuMemData_.group, "PPU Memory");

  //---

  auto cpu = machine_->getCPU();

  cpu->setDebugger(true);

  ppuMemData_.memArea->setFont(getFixedFont());
  ppuMemData_.memArea->setMemoryText();

  cpu->setDebugger(false);

  //---

  QColor cartridgeColor(180, 216, 180);
  QColor ramColor      (216, 180, 180);

  memoryArea_->setMemoryColor(0x0000, 0x2000, ramColor);
  memoryArea_->setMemoryColor(0x8000, 0x8000, cartridgeColor);
}

void
CQNESDbg::
addRightWidgets()
{
  CQ6502Dbg::addRightWidgets();

  //---

  auto addColHexEdit2 = [&](QGridLayout *layout, const QString &name, int row, int &col) {
    auto *hlayout = new QHBoxLayout;
    hlayout->addWidget(CQUtil::makeLabelWidget<QLabel>(name));
    auto edit = new CQ6502HexEdit(2);
    hlayout->addWidget(edit);
    layout->addLayout(hlayout, row, col); ++col;
    return edit;
  };

  auto addHexEdit2 = [&](QGridLayout *layout, const QString &name, int &row) {
    auto edit = new CQ6502HexEdit(2);
    layout->addWidget(CQUtil::makeLabelWidget<QLabel>(name), row, 0);
    layout->addWidget(edit, row, 1); ++row;
    return edit;
  };

  auto addColHexEdit4 = [&](QGridLayout *layout, const QString &name, int row, int &col) {
    auto *hlayout = new QHBoxLayout;
    hlayout->addWidget(CQUtil::makeLabelWidget<QLabel>(name));
    auto edit = new CQ6502HexEdit(4);
    hlayout->addWidget(edit);
    layout->addLayout(hlayout, row, col); ++col;
    return edit;
  };

  auto addHexEdit4 = [&](QGridLayout *layout, const QString &name, int &row) {
    auto edit = new CQ6502HexEdit(4);
    layout->addWidget(CQUtil::makeLabelWidget<QLabel>(name), row, 0);
    layout->addWidget(edit, row, 1); ++row;
    return edit;
  };

  auto addColLabel = [&](QGridLayout *layout, const QString &name, int row, int &col) {
    auto label = CQUtil::makeLabelWidget<QLabel>(name, "label");
    layout->addWidget(label, row, col, 1, 1); ++col;
    return label;
  };

  auto addColCheck = [&](QGridLayout *layout, const QString &name, int row, int &col,
                         int ncols=1) {
    auto check = CQUtil::makeLabelWidget<QCheckBox>(name);
    layout->addWidget(check, row, col, 1, ncols); col += ncols;
    return check;
  };

  auto addCheck = [&](QGridLayout *layout, const QString &name, int &row) {
    int col = 0; auto check = addColCheck(layout, name, row, col, 2); ++row;
    return check;
  };

  auto addDebugReadWriteChecks = [&](QGridLayout *layout, int &row) {
    auto label  = CQUtil::makeLabelWidget<QLabel   >("Debug");
    auto check1 = CQUtil::makeLabelWidget<QCheckBox>("Read");
    auto check2 = CQUtil::makeLabelWidget<QCheckBox>("Write");

    layout->addWidget(label , row, 0);
    layout->addWidget(check1, row, 1);
    layout->addWidget(check2, row, 2); ++row;

    return std::pair<QCheckBox*,QCheckBox*>(check1, check2);
  };

  //---

  // NES (Machine) group

  nesData_.group = CQUtil::makeLabelWidget<QGroupBox>("Enabled", "nesGroup");

  nesData_.group->setCheckable(true);

  //connect(nesData_.group, SIGNAL(toggled(bool)), this, SLOT(nesTraceSlot()));

  auto nesLayout = CQUtil::makeLayout<QGridLayout>(nesData_.group, 2, 2);

  int nesRow = 0, nesCol = 0;

  auto nesChecks = addDebugReadWriteChecks(nesLayout, nesRow);

  nesData_.debugReadCheck  = nesChecks.first;
  nesData_.debugWriteCheck = nesChecks.second;

  nesCol = 0;
  (void)                      addColLabel(nesLayout, "Stop on"     , nesRow, nesCol);
  nesData_.illegalJumpCheck = addColCheck(nesLayout, "Illegal Jump", nesRow, nesCol);
  nesData_.nmiCheck         = addColCheck(nesLayout, "NMI"         , nesRow, nesCol); ++nesRow;

  nesData_.nameTableEdit      = addHexEdit2(nesLayout, "Name Table", nesRow);
  nesData_.verticalWriteCheck = addCheck   (nesLayout, "Vertical Write", nesRow);

  nesCol = 0;
  (void)                           addColLabel   (nesLayout, "Screen" , nesRow, nesCol);
  nesData_.screenVisibleCheck    = addColCheck   (nesLayout, "Visible", nesRow, nesCol);
  nesData_.screenPatternAddrEdit = addColHexEdit4(nesLayout, "Addr"   , nesRow, nesCol); ++nesRow;

//nesData_.screenVisibleCheck    = addCheck   (nesLayout, "Screen Visible", nesRow);
//nesData_.screenPatternAddrEdit = addHexEdit4(nesLayout, "Screen Pattern Addr", nesRow);

  nesCol = 0;
  (void)                           addColLabel   (nesLayout, "Sprites", nesRow, nesCol);
  nesData_.spritesVisibleCheck   = addColCheck   (nesLayout, "Visible", nesRow, nesCol);
  nesData_.spritePatternAddrEdit = addColHexEdit4(nesLayout, "Addr"   , nesRow, nesCol); ++nesRow;

//nesData_.spritesVisibleCheck   = addCheck   (nesLayout, "Sprites Visible", nesRow);
//nesData_.spritePatternAddrEdit = addHexEdit4(nesLayout, "Sprite Pattern Addr", nesRow);

  nesData_.spriteSizeEdit    = addHexEdit2(nesLayout, "Sprite Size", nesRow);
//nesData_.hitInterruptCheck = addCheck   (nesLayout, "Sprite Hit Interrupt", nesRow);

  nesCol = 0;
  (void)                     addColLabel(nesLayout, "Mark"  , nesRow, nesCol);
  nesData_.imageMaskCheck  = addColCheck(nesLayout, "Image" , nesRow, nesCol);
  nesData_.spriteMaskCheck = addColCheck(nesLayout, "Sprite", nesRow, nesCol); ++nesRow;

  nesData_.blankInterruptCheck = addCheck(nesLayout, "VBlank Interrupt", nesRow);

  nesCol = 0;
  (void)                 addColLabel   (nesLayout, "Scroll", nesRow, nesCol);
  nesData_.scrollVEdit = addColHexEdit2(nesLayout, "V"     , nesRow, nesCol);
  nesData_.scrollHEdit = addColHexEdit2(nesLayout, "H"     , nesRow, nesCol); ++nesRow;

  nesLayout->setColumnStretch(3, 1);
  nesLayout->setRowStretch(nesRow, 1);

  connect(nesData_.debugReadCheck , SIGNAL(stateChanged(int)), this, SLOT(nesDebugReadSlot(int)));
  connect(nesData_.debugWriteCheck, SIGNAL(stateChanged(int)), this, SLOT(nesDebugWriteSlot(int)));

  addRightWidget(nesData_.group, "NES");

  //---

  // PPU group

  ppuData_.group = CQUtil::makeLabelWidget<QGroupBox>("Enabled", "ppuGroup");

  ppuData_.group->setCheckable(true);

  //connect(ppuData_.group, SIGNAL(toggled(bool)), this, SLOT(ppuTraceSlot()));

  auto ppuLayout = CQUtil::makeLayout<QGridLayout>(ppuData_.group, 2, 2);

  int ppuRow = 0;

  auto ppuChecks = addDebugReadWriteChecks(ppuLayout, ppuRow);

  ppuData_.debugReadCheck  = ppuChecks.first;
  ppuData_.debugWriteCheck = ppuChecks.second;

  ppuData_.scanLineCheck = addCheck(ppuLayout, "Scan Line", ppuRow);

  ppuLayout->setColumnStretch(3, 1);
  ppuLayout->setRowStretch(ppuRow, 1);

  connect(ppuData_.debugReadCheck , SIGNAL(stateChanged(int)), this, SLOT(ppuDebugReadSlot(int)));
  connect(ppuData_.debugWriteCheck, SIGNAL(stateChanged(int)), this, SLOT(ppuDebugWriteSlot(int)));
  connect(ppuData_.scanLineCheck  , SIGNAL(stateChanged(int)), this, SLOT(ppuScanLineSlot(int)));

  addRightWidget(ppuData_.group, "PPU");

  //---

  // Cartridge group

  cartData_.group = CQUtil::makeLabelWidget<QGroupBox>("Enabled", "cartGroup");

  cartData_.group->setCheckable(true);

  //connect(cartData_.group, SIGNAL(toggled(bool)), this, SLOT(cartTraceSlot()));

  auto cartLayout = CQUtil::makeLayout<QGridLayout>(cartData_.group, 2, 2);

  int cartRow = 0;

  cartData_.debugCheck     = addCheck   (cartLayout, "Debug", cartRow);
  cartData_.prgSizeEdit    = addHexEdit4(cartLayout, "ROM Size", cartRow);
  cartData_.chrSizeEdit    = addHexEdit4(cartLayout, "VROM Size", cartRow);
  cartData_.mirroringCheck = addCheck   (cartLayout, "Mirroring", cartRow);
  cartData_.prgRamSizeEdit = addHexEdit4(cartLayout, "RAM Size", cartRow);
  cartData_.mapperEdit     = addHexEdit2(cartLayout, "Mapper", cartRow);

  int cartCol = 0;
  (void)               addColLabel   (cartLayout, "Sprites", cartRow, cartCol);
  cartData_.chrLEdit = addColHexEdit2(cartLayout, "Lower"  , cartRow, cartCol);
  cartData_.chrHEdit = addColHexEdit2(cartLayout, "Upper"  , cartRow, cartCol); ++cartRow;

  cartLayout->setRowStretch(cartRow, 1);

  connect(cartData_.debugCheck, SIGNAL(stateChanged(int)), this, SLOT(cartDebugSlot(int)));

  connect(cartData_.chrLEdit, SIGNAL(valueChanged(unsigned int)),
          this, SLOT(nesChrLSlot(unsigned int)));
  connect(cartData_.chrHEdit, SIGNAL(valueChanged(unsigned int)),
          this, SLOT(nesChrHSlot(unsigned int)));

  addRightWidget(cartData_.group, "Cartridge");

  //---

  updateNESSlot();
}

void
CQNESDbg::
setFixedFont(const QFont &font)
{
  CQ6502Dbg::setFixedFont(font);

  ppuMemData_.memArea->setFont(font);
}

void
CQNESDbg::
setTraced(bool traced)
{
  CQ6502Dbg::setTraced(traced);

  // left
  ppuMemData_.group ->setChecked(traced);

  // right
  nesData_.group ->setChecked(traced);
  cartData_.group->setChecked(traced);
}

void
CQNESDbg::
updatePPUSlot(ushort addr, ushort /*len*/)
{
  if (ppuMemData_.group->isChecked()) {
    ppuMemData_.memArea->setMemoryLine(addr);

    ppuMemData_.memArea->updateText(addr);
  }
}

void
CQNESDbg::
nesChrLSlot(unsigned int page)
{
  auto cart = machine_->getCart();

  cart->setChrLPage(page);
}

void
CQNESDbg::
nesChrHSlot(unsigned int page)
{
  auto cart = machine_->getCart();

  cart->setChrHPage(page);
}

void
CQNESDbg::
nesDebugReadSlot(int state)
{
  auto cpu = machine_->getCPU();

  cpu->setDebugRead(state);
}

void
CQNESDbg::
nesDebugWriteSlot(int state)
{
  auto cpu = machine_->getCPU();

  cpu->setDebugWrite(state);
}

void
CQNESDbg::
ppuDebugReadSlot(int state)
{
  auto ppu = machine_->getPPU();

  ppu->setDebugRead(state);
}

void
CQNESDbg::
ppuDebugWriteSlot(int state)
{
  auto ppu = machine_->getPPU();

  ppu->setDebugWrite(state);
}

void
CQNESDbg::
ppuScanLineSlot(int state)
{
  auto ppu = machine_->getQPPU();

  ppu->setShowScanLine(state);
}

void
CQNESDbg::
cartDebugSlot(int state)
{
  auto cart = machine_->getCart();

  cart->setDebugWrite(state);
}

void
CQNESDbg::
updateNESSlot()
{
  auto *cpu = machine_->getCPU();
  auto *ppu = machine_->getPPU();

  nesData_.debugReadCheck ->setChecked(cpu->isDebugRead ());
  nesData_.debugWriteCheck->setChecked(cpu->isDebugWrite());

  nesData_.nameTableEdit     ->setValue  (ppu->nameTable());
  nesData_.verticalWriteCheck->setChecked(ppu->isVerticalWrite ());

  nesData_.screenVisibleCheck   ->setChecked(ppu->isScreenVisible ());
  nesData_.screenPatternAddrEdit->setValue  (ppu->screenPatternAddr());

  nesData_.spritesVisibleCheck  ->setChecked(ppu->isSpritesVisible());
  nesData_.spritePatternAddrEdit->setValue  (ppu->spritePatternAddr());
  nesData_.spriteSizeEdit       ->setValue  (ppu->spriteSize());
//nesData_.hitInterruptCheck    ->setChecked(ppu->isSpriteInterrupt());

  nesData_.imageMaskCheck ->setChecked(ppu->isImageMasked());
  nesData_.spriteMaskCheck->setChecked(ppu->isSpriteMasked());

  nesData_.blankInterruptCheck->setChecked(ppu->isBlankInterrupt());

  nesData_.scrollVEdit->setValue  (ppu->scrollV());
  nesData_.scrollHEdit->setValue  (ppu->scrollH());

  //---

  ppuData_.debugReadCheck ->setChecked(ppu->isDebugRead ());
  ppuData_.debugWriteCheck->setChecked(ppu->isDebugWrite());

  //---

  auto cart = machine_->getCart();

  cartData_.debugCheck    ->setChecked(cart->isDebugWrite());
  cartData_.prgSizeEdit   ->setValue  (cart->prgSize());
  cartData_.chrSizeEdit   ->setValue  (cart->chrSize());
  cartData_.mirroringCheck->setChecked(cart->isMirroring());
  cartData_.prgRamSizeEdit->setValue  (cart->prgRamSize());
  cartData_.mapperEdit    ->setValue  (cart->mapper());
}

void
CQNESDbg::
ppuMemEnabledSlot()
{
  if (ppuMemData_.group->isChecked()) {
    ppuMemData_.memArea->setMemoryText();
  }
}

void
CQNESDbg::
illegalJumpSlot()
{
  if (nesData_.illegalJumpCheck->isChecked())
    forceHalt();
}

void
CQNESDbg::
nmiSlot()
{
  if (nesData_.nmiCheck->isChecked())
    forceHalt();
}

//------

CQNESPPUMem::
CQNESPPUMem(QMachine *machine) :
 machine_(machine)
{
  QColor patternColor  (180, 216, 180);
  QColor nameTableColor(216, 180, 180);
  QColor paletteColor  (180, 180, 216);

  setMemoryColor(0x0000, 0x2000, patternColor);
  setMemoryColor(0x2000, 0x1000, nameTableColor);
  setMemoryColor(0x3F00, 0x0100, paletteColor);
}

uint
CQNESPPUMem::
getCurrentAddress() const
{
  return 0;
}

void
CQNESPPUMem::
setCurrentAddress(uint /*addr*/)
{
}

bool
CQNESPPUMem::
isReadOnlyAddr(uint /*addr*/, uint /*len*/) const
{
  return false;
}

bool
CQNESPPUMem::
isScreenAddr(uint /*addr*/, uint /*len*/) const
{
  return false;
}

uchar
CQNESPPUMem::
getByte(uint addr) const
{
  auto *ppu = machine_->getPPU();

  return ppu->getByte(addr);
}
