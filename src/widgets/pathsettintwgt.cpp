#include "pathsettintwgt.h"
#include "../common/settings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QFileDialog>
#include <QDir>

PathSettingWgt::PathSettingWgt(QWidget* parent):DWidget(parent)
{
    init();
    onSaveIdChanged(Settings::instance()->getSavePathId());

    // 若当前非自定义路径，则单独设置自定义文档目录
    if (CustomBox != Settings::instance()->getSavePathId()) {
        // 获取当前用户定义路径，若无则设置为文档目录
        setEditText(Settings::instance()->getSavePath(CustomBox));
    }
}

PathSettingWgt::~PathSettingWgt()
{

}

void PathSettingWgt::onSaveIdChanged(int id)
{
    switch (id) {
    case CurFileBox:{
        m_curFileBox->setChecked(true);
        m_customBtn->setEnabled(false);
        break;
    }
    case LastOptBox:{
        m_lastOptBox->setChecked(true);
        m_customBtn->setEnabled(false);
        break;
    }
    case CustomBox:{
        m_customBox->setChecked(true);
        m_customBtn->setEnabled(true);
        setEditText(Settings::instance()->getSavePath(CustomBox));
        break;
    }
    default:
        break;
    }
}

void PathSettingWgt::init()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_group = new QButtonGroup(this);
    layout->setContentsMargins(0,0,0,0);
    m_curFileBox = new DCheckBox(this);
    m_lastOptBox = new DCheckBox(this);
    m_customBox = new DCheckBox(this);
    m_customEdit = new DLineEdit(this);
    m_customBtn = new DPushButton(this);
    m_group->addButton(m_lastOptBox,LastOptBox);
    m_group->addButton(m_curFileBox,CurFileBox);
    m_group->addButton(m_customBox,CustomBox);

    m_lastOptBox->setText("remember the last opt path");
    m_curFileBox->setText("be consistent with it cur file path");
    m_customBox->setText("custom the file path");
    m_customBtn->setText("...");
    m_customBtn->setEnabled(false);
    m_customEdit->setDisabled(true);
    m_customEdit->setClearButtonEnabled(false);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addWidget(m_customBox);
    hlayout->addWidget(m_customEdit);
    hlayout->addWidget(m_customBtn);

    layout->addWidget(m_lastOptBox);
    layout->addWidget(m_curFileBox);
    layout->addLayout(hlayout);

    connections();
}

void PathSettingWgt::connections()
{
    connect(m_group,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),this,&PathSettingWgt::onBoxClicked);
    connect(m_customBtn,&DPushButton::clicked,this,&PathSettingWgt::onBtnClicked);
}

QString PathSettingWgt::checkCustomPath(const QString &path) const
{
    // 判断当前自定义路径是否存在，不存在则返回文档路径
    if (path.isEmpty()) {
        return QDir::homePath() + "/Documents";
    }
    return path;
}

void PathSettingWgt::setEditText(const QString& text)
{
    // 校验路径是否合法
    QString checkText = checkCustomPath(text);

    QFontMetrics metrics(m_customEdit->font());
    Qt::TextElideMode em = Qt::TextElideMode::ElideMiddle;
    m_customEdit->setText(metrics.elidedText(checkText, em, 175));
}

void PathSettingWgt::onBoxClicked(int id)
{
    switch (id) {
    case CurFileBox:{
        Settings::instance()->setSavePathId(CurFileBox);
        m_customBtn->setEnabled(false);
        break;
    }
    case LastOptBox:{
        Settings::instance()->setSavePathId(LastOptBox);
        m_customBtn->setEnabled(false);
        break;
    }
    case CustomBox:{
        Settings::instance()->setSavePathId(CustomBox);
        setEditText(Settings::instance()->getSavePath(CustomBox));
        m_customBtn->setEnabled(true);
        break;
    }
    default:
        break;
    }
}

void PathSettingWgt::onBtnClicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QString path = Settings::instance()->getSavePath(PathSettingWgt::CustomBox);
    if(!QDir(path).exists() || path.isEmpty()){
        path = QDir::homePath() + "/Documents";
    }
    dialog.setDirectory(path);
    const int mode = dialog.exec();
    if (mode != QDialog::Accepted) {
        return;
    }

    path = dialog.selectedFiles().at(0);
    setEditText(path);
    Settings::instance()->setSavePath(PathSettingWgt::CustomBox,path);
}
