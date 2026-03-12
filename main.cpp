#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>
#include <QTimer>
#include <QElapsedTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QRandomGenerator>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QFontDatabase>
#include <QFont>
#include <QDialog>
#include <QKeySequence>
#include <QPixmap>
#include <QImage>
#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>
#include <cmath>
#include <functional>
#include <QThread>
#include <QMutex>
#include <QAtomicInt>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <dwmapi.h>
#endif


static QPixmap iconPixmap(const QString& filename, int size = 36) {
    

    QString resourcePath = ":/icons/" + filename;
    QString diskPath     = QCoreApplication::applicationDirPath() + "/icons/" + filename;
    QImage img;
    if (!img.load(resourcePath) && !img.load(diskPath)) {
        QPixmap blank(size, size);
        blank.fill(Qt::transparent);
        return blank;
    }
    

    img = img.convertToFormat(QImage::Format_ARGB32);
    

    

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QColor c = img.pixelColor(x, y);
            if (c.alpha() > 0) {
                c.setRed  (255 - c.red());
                c.setGreen(255 - c.green());
                c.setBlue (255 - c.blue());
                img.setPixelColor(x, y, c);
            }
        }
    }
    return QPixmap::fromImage(img).scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


static const QColor C_BG        ("#141414");   

static const QColor C_TOPBAR    ("#0F0F0F");   

static const QColor C_PANEL     ("#1C1C1C");   

static const QColor C_CARD      ("#212121");   

static const QColor C_CARD_HOV  ("#2C2C2C");   

static const QColor C_CARD_SEL  ("#262626");   

static const QColor C_INPUT     ("#181818");   

static const QColor C_BORDER    ("#3A3A3A");   

static const QColor C_BORDER_S  ("#FFFFFF");   

static const QColor C_BORDER_D  ("#262626");   

static const QColor C_TEXT      ("#F8F8F8");   

static const QColor C_TEXT_DIM  ("#888888");   

static const QColor C_TEXT_HINT ("#505050");   

static const QColor C_WHITE     ("#FFFFFF");   

static const QColor C_WHITE_D   ("#484848");   

static const QColor C_RED       ("#CC2020");   

static const QColor C_RED_HOV   ("#E03030");


class Panel : public QWidget {
public:
    explicit Panel(QWidget* p = nullptr) : QWidget(p) {}
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter pt(this);
        pt.setRenderHint(QPainter::Antialiasing);
        pt.setPen(QPen(C_BORDER_D, 1.0));
        pt.setBrush(C_PANEL);
        pt.drawRoundedRect(QRectF(0.5, 0.5, width()-1, height()-1), 8.0, 8.0);
    }
};


class SnowWidget : public QWidget {
    Q_OBJECT
public:
    struct Flake { float x, y, vx, vy, size, opacity, wobble, wobbleSpeed; };

    explicit SnowWidget(QWidget* parent = nullptr) : QWidget(parent) {
        

        

        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAutoFillBackground(false);

        auto* rng = QRandomGenerator::global();
        for (int i = 0; i < 90; i++) {
            Flake f;
            f.x           = float(rng->bounded(860));
            f.y           = float(rng->bounded(640));
            f.vx          = float(rng->generateDouble() - 0.5) * 0.5f;
            f.vy          = 0.4f + float(rng->generateDouble()) * 0.8f;
            f.size        = 2.0f + float(rng->generateDouble()) * 3.0f;
            f.opacity     = 0.18f + float(rng->generateDouble()) * 0.42f;
            f.wobble      = float(rng->generateDouble()) * 6.283f;
            f.wobbleSpeed = 0.012f + float(rng->generateDouble()) * 0.018f;
            m_flakes.append(f);
        }
        m_elapsed.start();
        m_timer = new QTimer(this);
        m_timer->setTimerType(Qt::PreciseTimer); 

        connect(m_timer, &QTimer::timeout, this, &SnowWidget::step);
        m_timer->start(8);
    }

private slots:
    void step() {
        

        float dt = float(m_elapsed.restart()) / 8.0f;
        dt = qBound(0.1f, dt, 2.0f);
        auto* rng = QRandomGenerator::global();
        int W = width(), H = height();
        if (W == 0 || H == 0) return;
        for (auto& f : m_flakes) {
            f.wobble += f.wobbleSpeed * dt;
            f.x += (f.vx + std::sin(f.wobble) * 0.35f) * dt;
            f.y += f.vy * dt;
            if (f.y > H + 10) {
                f.y = -10.f;
                f.x = float(rng->bounded(W));
                f.vx = float(rng->generateDouble() - 0.5) * 0.5f;
                f.vy = 0.4f + float(rng->generateDouble()) * 0.8f;
            }
            if (f.x < -5) f.x += W + 5;
            if (f.x > W + 5) f.x -= W + 5;
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (const auto& f : m_flakes) {
            QColor c(255, 255, 255);
            c.setAlphaF(f.opacity);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawEllipse(QPointF(f.x, f.y), f.size * 0.5f, f.size * 0.5f);
        }
    }

private:
    QList<Flake>  m_flakes;
    QTimer*       m_timer = nullptr;
    QElapsedTimer m_elapsed;
};


class AnimatedStack : public QStackedWidget {
public:
    explicit AnimatedStack(QWidget* parent = nullptr) : QStackedWidget(parent) {}
    

    void switchTo(int idx) { setCurrentIndex(idx); }
};


class ToggleSwitch : public QWidget {
    Q_OBJECT
public:
    explicit ToggleSwitch(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(46, 24);
        setCursor(Qt::PointingHandCursor);
        m_timer = new QTimer(this);
        m_timer->setTimerType(Qt::PreciseTimer);
        m_timer->setInterval(8);
        connect(m_timer, &QTimer::timeout, this, [this]{
            

            const float duration = 160.0f;
            float progress = qBound(0.0f, float(m_elapsed.elapsed()) / duration, 1.0f);
            

            float ease = progress < 0.5f
                ? 4.0f * progress * progress * progress
                : 1.0f - std::pow(-2.0f * progress + 2.0f, 3.0f) / 2.0f;
            m_t = m_on ? ease : (1.0f - ease);
            update();
            if (progress >= 1.0f) m_timer->stop();
        });
        m_elapsed.start();
    }
    bool isOn() const { return m_on; }
    void setOn(bool v) {
        if (m_on == v) return;
        m_on = v;
        m_elapsed.restart();
        m_timer->start();
        emit toggled(v);
    }
    QSize sizeHint() const override { return QSize(46, 24); }
signals:
    void toggled(bool on);
protected:
    void mousePressEvent(QMouseEvent*) override { setOn(!m_on); }
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        float W = width(), H = height(), R = H * 0.5f;
        float t = m_t;
        

        int tb = int(0x18 + t * (0x28 - 0x18));
        p.setBrush(QColor(tb, tb, tb));
        

        int bv = int(0x26 + t * (0xFF - 0x26));
        p.setPen(QPen(QColor(bv, bv, bv), 1.2f));
        p.drawRoundedRect(QRectF(0.5, 0.5, W-1, H-1), R, R);
        

        float kx = R + t * (W - H);
        int kv = int(0x88 + t * (0xFF - 0x88));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(kv, kv, kv));
        p.drawEllipse(QPointF(kx, H * 0.5f), R - 2.8f, R - 2.8f);
    }
private:
    bool          m_on    = false;
    float         m_t     = 0.f;
    QTimer*       m_timer = nullptr;
    QElapsedTimer m_elapsed;
};


class StatusDot : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float activeBlend READ activeBlend WRITE setActiveBlend)
public:
    explicit StatusDot(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(16, 16);
        

        m_timer = new QTimer(this);
        m_timer->setTimerType(Qt::PreciseTimer);
        connect(m_timer, &QTimer::timeout, this, &StatusDot::onTick);
        m_timer->start(8);
        

        m_blendAnim = new QPropertyAnimation(this, "activeBlend", this);
        m_blendAnim->setDuration(350);
        m_blendAnim->setEasingCurve(QEasingCurve::InOutCubic);
        m_elapsed.start();
    }
    float activeBlend() const { return m_blend; }
    void setActiveBlend(float v) { m_blend = v; update(); }
    void setActive(bool v) {
        m_active = v;
        m_blendAnim->stop();
        m_blendAnim->setStartValue(m_blend);
        m_blendAnim->setEndValue(v ? 1.0f : 0.0f);
        m_blendAnim->start();
    }
    QSize sizeHint() const override { return QSize(16, 16); }
private slots:
    void onTick() {
        float dt = float(m_elapsed.restart()) / 8.0f;
        dt = qBound(0.1f, dt, 2.0f);
        m_phase += 0.03f * dt;
        if (m_phase > 6.2832f) m_phase -= 6.2832f;
        update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        float cx = width() * 0.5f, cy = height() * 0.5f;
        float pulse = 0.5f + 0.5f * std::sin(m_phase);
        

        if (m_blend > 0.01f) {
            QColor g1 = C_WHITE; g1.setAlphaF(0.08f * pulse * m_blend);
            QColor g2 = C_WHITE; g2.setAlphaF(0.16f * pulse * m_blend);
            p.setPen(Qt::NoPen);
            p.setBrush(g1); p.drawEllipse(QPointF(cx,cy), 7.f, 7.f);
            p.setBrush(g2); p.drawEllipse(QPointF(cx,cy), 5.f, 5.f);
        }
        p.setPen(Qt::NoPen);
        p.setBrush(C_WHITE);
        p.drawEllipse(QPointF(cx,cy), 3.5f, 3.5f);
    }
private:
    bool    m_active = true;
    float   m_blend  = 0.f;
    float   m_phase  = 0.f;
    QTimer*             m_timer     = nullptr;
    QPropertyAnimation* m_blendAnim = nullptr;
    QElapsedTimer       m_elapsed;
};


class InfoBox : public QWidget {
public:
    explicit InfoBox(const QString& labelText, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setFixedHeight(52);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto* lay = new QVBoxLayout(this);
        lay->setContentsMargins(10, 6, 10, 6);
        lay->setSpacing(2);
        m_label = new QLabel(labelText, this);
        m_label->setStyleSheet("color:#505050;font-size:10px;letter-spacing:0.5px;background:transparent;");
        m_value = new QLabel("â", this);
        m_value->setStyleSheet("color:#F0F0F0;font-size:13px;font-weight:500;background:transparent;");
        lay->addWidget(m_label);
        lay->addWidget(m_value);
    }
    void    setValue(const QString& v) { m_value->setText(v); }
    QLabel* valueLabel()               { return m_value; }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(C_BORDER_D, 1.0));
        p.setBrush(C_PANEL);
        p.drawRoundedRect(QRectF(0.5, 0.5, width()-1, height()-1), 7.0, 7.0);
    }
private:
    QLabel* m_label = nullptr;
    QLabel* m_value = nullptr;
};


class NavButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(float uline READ uline WRITE setUline)
public:
    explicit NavButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {
        setFixedHeight(34);
        setMinimumWidth(100);
        setCursor(Qt::PointingHandCursor);
        setFlat(true);
        setAttribute(Qt::WA_Hover);
        m_anim = new QPropertyAnimation(this, "uline", this);
        m_anim->setDuration(180);
        m_anim->setEasingCurve(QEasingCurve::OutCubic);
    }
    float uline() const { return m_uline; }
    void  setUline(float v) { m_uline = v; update(); }
    void  setActive(bool active) {
        m_active = active;
        m_anim->stop();
        m_anim->setStartValue(m_uline);
        m_anim->setEndValue(active ? 1.0f : 0.0f);
        m_anim->start();
        update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        bool hov = underMouse();
        QColor col = m_active ? C_WHITE : (hov ? C_TEXT : C_TEXT_DIM);
        QFont f = font(); f.setPixelSize(12);
        f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
        p.setFont(f); p.setPen(col);
        p.drawText(rect(), Qt::AlignCenter, text());
        if (m_uline > 0.01f) {
            float hw = width() * 0.38f * m_uline;
            float cx = width() * 0.5f;
            QColor wc = C_WHITE; wc.setAlphaF(m_uline);
            p.setPen(Qt::NoPen); p.setBrush(wc);
            p.drawRoundedRect(QRectF(cx-hw, height()-2.5f, hw*2, 2.5f), 1.f, 1.f);
        }
    }
    void enterEvent(QEnterEvent*) override { update(); }
    void leaveEvent(QEvent*)      override { update(); }
private:
    float  m_uline  = 0.f;
    bool   m_active = false;
    QPropertyAnimation* m_anim = nullptr;
};


class MacroCard : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float glow READ glow WRITE setGlow)
public:
    enum IconType { Arrow, Bolt, Crosshair, Grid };

    MacroCard(const QString& name, IconType icon, int id, QWidget* parent = nullptr)
        : QWidget(parent), m_name(name), m_icon(icon), m_id(id)
    {
        setFixedHeight(100);
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_anim = new QPropertyAnimation(this, "glow", this);
        m_anim->setDuration(220);
        m_anim->setEasingCurve(QEasingCurve::OutCubic);
    }
    float glow() const { return m_glow; }
    void  setGlow(float v) { m_glow = v; update(); }
    void  setSelected(bool s) {
        m_selected = s;
        m_anim->stop();
        m_anim->setStartValue(m_glow);
        m_anim->setEndValue(s ? 1.0f : 0.0f);
        m_anim->start();
        update();
    }
    bool isSelected() const { return m_selected; }
    int  cardId()     const { return m_id; }

signals:
    void clicked(int id);

protected:
    void mousePressEvent(QMouseEvent*) override { emit clicked(m_id); }
    void enterEvent(QEnterEvent*)      override { m_hov = true;  update(); }
    void leaveEvent(QEvent*)           override { m_hov = false; update(); }
    void paintEvent(QPaintEvent*)      override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r(1.0, 1.0, width()-2, height()-2);

        

        if (m_glow > 0.02f) {
            for (int i = 4; i >= 1; i--) {
                float e = i * 2.5f;
                QColor gc = C_WHITE;
                gc.setAlphaF(0.04f * m_glow * (float)i / 4.f);
                p.setPen(Qt::NoPen); p.setBrush(gc);
                p.drawRoundedRect(r.adjusted(-e,-e,e,e), 12.f+e, 12.f+e);
            }
        }

        

        QColor fill = m_selected ? QColor("#272727")
                      : m_hov     ? QColor("#252525")
                                 : C_CARD;
        p.setBrush(fill);

        

        if (m_selected) {
            QColor bc = C_WHITE; bc.setAlphaF(0.55f + 0.45f * m_glow);
            p.setPen(QPen(bc, 1.5));
        } else {
            QColor bc = m_hov ? C_BORDER : C_BORDER_D;
            p.setPen(QPen(bc, 1.0));
        }
        p.drawRoundedRect(r, 10.0, 10.0);

        

        int iconSize  = 32;
        int iconZoneH = int(height() * 0.68f);
        QPixmap pm;
        switch (m_icon) {
        case Arrow:     pm = iconPixmap("drag.png",      iconSize); break;
        case Bolt:      pm = iconPixmap("bolt.png",      iconSize); break;
        case Crosshair: pm = iconPixmap("crosshair.png", iconSize); break;
        case Grid:      pm = iconPixmap("build.png",     iconSize); break;
        }
        

        float iconAlpha = m_selected ? 1.0f : (m_hov ? 0.65f : 0.45f);
        if (iconAlpha < 0.99f) {
            QImage img = pm.toImage().convertToFormat(QImage::Format_ARGB32);
            for (int y2 = 0; y2 < img.height(); y2++)
                for (int x2 = 0; x2 < img.width(); x2++) {
                    QColor c = img.pixelColor(x2, y2);
                    c.setAlpha(int(c.alpha() * iconAlpha));
                    img.setPixelColor(x2, y2, c);
                }
            pm = QPixmap::fromImage(img);
        }
        int px = (width()     - iconSize) / 2;
        int py = (iconZoneH   - iconSize) / 2;
        p.drawPixmap(px, py, pm);

        

        QFont fn = font(); fn.setPixelSize(12);
        fn.setWeight(m_selected ? QFont::DemiBold : QFont::Normal);
        fn.setLetterSpacing(QFont::AbsoluteSpacing, 0.3);
        p.setFont(fn);
        QColor textCol = m_selected ? C_TEXT : (m_hov ? QColor("#AAAAAA") : C_TEXT_DIM);
        p.setPen(textCol);
        int textZoneTop = iconZoneH;
        int textZoneH   = height() - textZoneTop;
        p.drawText(QRectF(6, textZoneTop, width()-12, textZoneH),
                   Qt::AlignHCenter | Qt::AlignVCenter, m_name);
    }

private:
    QString  m_name;
    IconType m_icon;
    int      m_id       = 0;
    bool     m_selected = false;
    bool     m_hov      = false;
    float    m_glow     = 0.f;
    QPropertyAnimation* m_anim = nullptr;
};


class KeyCaptureDialog : public QDialog {
    Q_OBJECT
public:
    explicit KeyCaptureDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Set Keybind");
        setFixedSize(280, 130);
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setStyleSheet("background:#1C1C1C;border:1px solid #3A3A3A;border-radius:8px;");

        auto* lay = new QVBoxLayout(this);
        lay->setContentsMargins(20, 20, 20, 20);
        lay->setSpacing(10);

        m_lbl = new QLabel("Press any key or mouse button...");
        m_lbl->setAlignment(Qt::AlignCenter);
        m_lbl->setStyleSheet("color:#F0F0F0;font-size:14px;font-weight:600;background:transparent;");
        lay->addWidget(m_lbl);

        auto* sub = new QLabel("Press Escape to cancel");
        sub->setAlignment(Qt::AlignCenter);
        sub->setStyleSheet("color:#444444;font-size:11px;background:transparent;");
        lay->addWidget(sub);

        

        m_poll = new QTimer(this);
        m_poll->setInterval(16);
        connect(m_poll, &QTimer::timeout, this, &KeyCaptureDialog::pollMouse);
        m_poll->start();
    }

    ~KeyCaptureDialog() override {}

    QString capturedKey() const { return m_key; }

protected:
    void keyPressEvent(QKeyEvent* e) override {
        if (e->key() == Qt::Key_Escape) { reject(); return; }
        QString keyStr;
        switch (e->key()) {
        

        case Qt::Key_Delete:    keyStr = "DEL";   break;
        case Qt::Key_Insert:    keyStr = "INS";   break;
        case Qt::Key_Home:      keyStr = "HOME";  break;
        case Qt::Key_End:       keyStr = "END";   break;
        case Qt::Key_PageUp:    keyStr = "PGUP";  break;
        case Qt::Key_PageDown:  keyStr = "PGDN";  break;
        case Qt::Key_Left:      keyStr = "LEFT";  break;
        case Qt::Key_Right:     keyStr = "RIGHT"; break;
        case Qt::Key_Up:        keyStr = "UP";    break;
        case Qt::Key_Down:      keyStr = "DOWN";  break;
        

        case Qt::Key_F1:  case Qt::Key_F2:  case Qt::Key_F3:  case Qt::Key_F4:
        case Qt::Key_F5:  case Qt::Key_F6:  case Qt::Key_F7:  case Qt::Key_F8:
        case Qt::Key_F9:  case Qt::Key_F10: case Qt::Key_F11: case Qt::Key_F12:
        case Qt::Key_F13: case Qt::Key_F14: case Qt::Key_F15: case Qt::Key_F16:
        case Qt::Key_F17: case Qt::Key_F18: case Qt::Key_F19: case Qt::Key_F20:
        case Qt::Key_F21: case Qt::Key_F22: case Qt::Key_F23: case Qt::Key_F24:
            keyStr = QString("F%1").arg(e->key() - Qt::Key_F1 + 1); break;
        

        case Qt::Key_Shift:     keyStr = "SHIFT"; break;
        case Qt::Key_Control:   keyStr = "CTRL";  break;
        case Qt::Key_Alt:       keyStr = "ALT";   break;
        case Qt::Key_Meta:      keyStr = "WIN";   break;
        

        case Qt::Key_Tab:       keyStr = "TAB";   break;
        case Qt::Key_Space:     keyStr = "SPACE"; break;
        case Qt::Key_Return:
        case Qt::Key_Enter:     keyStr = "ENTER"; break;
        case Qt::Key_Backspace: keyStr = "BKSP";  break;
        case Qt::Key_CapsLock:  keyStr = "CAPS";  break;
        case Qt::Key_NumLock:   keyStr = "NUMLOCK"; break;
        case Qt::Key_ScrollLock: keyStr = "SCROLLLOCK"; break;
        case Qt::Key_Pause:     keyStr = "PAUSE"; break;
        case Qt::Key_Print:     keyStr = "PRINT"; break;
        

        

        case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3:
        case Qt::Key_4: case Qt::Key_5: case Qt::Key_6: case Qt::Key_7:
        case Qt::Key_8: case Qt::Key_9:
            if (e->modifiers() & Qt::KeypadModifier)
                keyStr = QString("NUMPAD%1").arg(e->key() - Qt::Key_0);
            else
                keyStr = QString(QChar(e->key()));
            break;
        case Qt::Key_Asterisk:
            if (e->modifiers() & Qt::KeypadModifier) keyStr = "NUM+*";
            else keyStr = "*";
            break;
        case Qt::Key_Plus:
            if (e->modifiers() & Qt::KeypadModifier) keyStr = "NUM++";
            else keyStr = "+";
            break;
        case Qt::Key_Minus:
            if (e->modifiers() & Qt::KeypadModifier) keyStr = "NUM+-";
            else keyStr = "-";
            break;
        case Qt::Key_Period:
            if (e->modifiers() & Qt::KeypadModifier) keyStr = "NUM+.";
            else keyStr = ".";
            break;
        case Qt::Key_Slash:
            if (e->modifiers() & Qt::KeypadModifier) keyStr = "NUM+/";
            else keyStr = "/";
            break;
        default:
            keyStr = QKeySequence(e->key()).toString().toUpper();
            break;
        }
        if (!keyStr.isEmpty()) { m_key = keyStr; m_poll->stop(); accept(); }
    }

private slots:
    void pollMouse() {
        struct { int vk; const char* name; } btns[] = {
                    { VK_LBUTTON,  "MB1" },
                    { VK_RBUTTON,  "MB2" },
                    { VK_MBUTTON,  "MB3" },
                    { VK_XBUTTON1, "MB4" },
                    { VK_XBUTTON2, "MB5" },
                    };
        for (auto& b : btns) {
            if (GetAsyncKeyState(b.vk) & 0x8000) {
                

                while (GetAsyncKeyState(b.vk) & 0x8000) QThread::msleep(10);
                m_key = b.name;
                m_poll->stop();
                accept();
                return;
            }
        }
    }

private:
    QString  m_key;
    QLabel*  m_lbl  = nullptr;
    QTimer*  m_poll = nullptr;
};

class KeyBindButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(float hov READ hov WRITE setHov)
public:
    explicit KeyBindButton(const QString& key, QWidget* parent = nullptr)
        : QPushButton(key, parent)
    {
        setFixedSize(90, 28);
        setCursor(Qt::PointingHandCursor);
        setAttribute(Qt::WA_Hover);
        m_hovAnim = new QPropertyAnimation(this, "hov", this);
        m_hovAnim->setDuration(150);
        m_hovAnim->setEasingCurve(QEasingCurve::OutCubic);
        setStyleSheet("QPushButton { background:transparent; border:none; }");
        connect(this, &QPushButton::clicked, this, &KeyBindButton::capture);
    }

    float hov() const { return m_hov; }
    void setHov(float v) { m_hov = v; update(); }

    void setKeySilent(const QString& k) {
        QSignalBlocker b(this);
        setText(k);
    }

signals:
    void keyChanged(const QString& key);

protected:
    void enterEvent(QEnterEvent*) override {
        m_hovAnim->stop();
        m_hovAnim->setStartValue(m_hov);
        m_hovAnim->setEndValue(1.0f);
        m_hovAnim->start();
    }
    void leaveEvent(QEvent*) override {
        m_hovAnim->stop();
        m_hovAnim->setStartValue(m_hov);
        m_hovAnim->setEndValue(0.0f);
        m_hovAnim->start();
    }
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        

        int bg = int(0x18 + m_hov * (0x26 - 0x18));
        QColor bgCol(bg, bg, bg);
        QColor borderCol;
        {
            int r = int(0x3A + m_hov * (0xFF - 0x3A));
            int g = int(0x3A + m_hov * (0xFF - 0x3A));
            int bv = int(0x3A + m_hov * (0xFF - 0x3A));
            borderCol = QColor(r, g, bv);
        }
        p.setBrush(bgCol);
        p.setPen(QPen(borderCol, 1.0));
        p.drawRoundedRect(QRectF(0.5, 0.5, width()-1, height()-1), 5.0, 5.0);
        QFont f = font(); f.setPixelSize(13); f.setFamily("Segoe UI");
        p.setFont(f);
        QColor textCol(0xF0 + int(m_hov * 15), 0xF0 + int(m_hov * 15), 0xF0 + int(m_hov * 15));
        p.setPen(textCol);
        p.drawText(rect(), Qt::AlignCenter, text());
    }

private slots:
    void capture() {
        KeyCaptureDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted && !dlg.capturedKey().isEmpty()) {
            setText(dlg.capturedKey());
            emit keyChanged(dlg.capturedKey());
        }
    }

private:
    float m_hov = 0.f;
    QPropertyAnimation* m_hovAnim = nullptr;
};


static QString inputCSS() {
    return
        "QLineEdit, QSpinBox {"
        "  background:#181818; color:#F0F0F0;"
        "  border:1px solid #3A3A3A; border-radius:5px;"
        "  padding:5px 8px; font-size:13px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus { border-color:#FFFFFF; }"
        "QSpinBox::up-button, QSpinBox::down-button { width:0; }"
        "QSlider::groove:horizontal {"
        "  height:4px; background:#181818;"
        "  border-radius:2px; border:1px solid #3A3A3A;"
        "}"
        "QSlider::handle:horizontal {"
        "  background:#FFFFFF; width:12px; height:12px;"
        "  margin:-5px 0; border-radius:6px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background:#404040; border-radius:2px;"
        "}";
}

static QLabel* secLabel(const QString& txt) {
    auto* l = new QLabel(txt);
    l->setStyleSheet(
        "color:#F0F0F0; font-size:12px; font-weight:600;"
        "border-left:3px solid #FFFFFF; padding-left:7px;"
        "background:transparent;");
    return l;
}

static QLabel* dimLabel(const QString& txt) {
    auto* l = new QLabel(txt);
    l->setStyleSheet("color:#888888; font-size:11px; background:transparent;");
    return l;
}

static QFrame* hline() {
    auto* f = new QFrame;
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("color:#1A1A1A;");
    f->setFixedHeight(1);
    return f;
}


static WORD keyNameToVK(const QString& name) {
    QString k = name.toUpper().trimmed();

    

    if (k == "MB1") return VK_LBUTTON;
    if (k == "MB2") return VK_RBUTTON;
    if (k == "MB3") return VK_MBUTTON;
    if (k == "MB4" || k == "XBUTTON1") return VK_XBUTTON1;
    if (k == "MB5" || k == "XBUTTON2") return VK_XBUTTON2;

    

    if (k.startsWith("F")) {
        bool ok; int n = k.mid(1).toInt(&ok);
        if (ok && n >= 1 && n <= 24) return (WORD)(VK_F1 + n - 1);
    }

    

    if (k == "SPACE")      return VK_SPACE;
    if (k == "ENTER" || k == "RETURN") return VK_RETURN;
    if (k == "TAB")        return VK_TAB;
    if (k == "SHIFT")      return VK_SHIFT;
    if (k == "CTRL" || k == "CONTROL") return VK_CONTROL;
    if (k == "ALT" || k == "MENU")     return VK_MENU;
    if (k == "WIN" || k == "META")     return VK_LWIN;
    if (k == "DEL" || k == "DELETE")   return VK_DELETE;
    if (k == "INS" || k == "INSERT")   return VK_INSERT;
    if (k == "HOME")       return VK_HOME;
    if (k == "END")        return VK_END;
    if (k == "PGUP" || k == "PAGEUP"  || k == "PG UP")  return VK_PRIOR;
    if (k == "PGDN" || k == "PAGEDOWN"|| k == "PG DOWN") return VK_NEXT;
    if (k == "LEFT")       return VK_LEFT;
    if (k == "RIGHT")      return VK_RIGHT;
    if (k == "UP")         return VK_UP;
    if (k == "DOWN")       return VK_DOWN;
    if (k == "BKSP" || k == "BACKSPACE") return VK_BACK;
    if (k == "ESC" || k == "ESCAPE")   return VK_ESCAPE;
    if (k == "CAPS" || k == "CAPSLOCK") return VK_CAPITAL;
    if (k == "NUMLOCK")    return VK_NUMLOCK;
    if (k == "SCROLLLOCK") return VK_SCROLL;
    if (k == "PAUSE")      return VK_PAUSE;
    if (k == "PRINT" || k == "PRINTSCREEN") return VK_SNAPSHOT;

    

    if (k == "NUM+0" || k == "NUMPAD0") return VK_NUMPAD0;
    if (k == "NUM+1" || k == "NUMPAD1") return VK_NUMPAD1;
    if (k == "NUM+2" || k == "NUMPAD2") return VK_NUMPAD2;
    if (k == "NUM+3" || k == "NUMPAD3") return VK_NUMPAD3;
    if (k == "NUM+4" || k == "NUMPAD4") return VK_NUMPAD4;
    if (k == "NUM+5" || k == "NUMPAD5") return VK_NUMPAD5;
    if (k == "NUM+6" || k == "NUMPAD6") return VK_NUMPAD6;
    if (k == "NUM+7" || k == "NUMPAD7") return VK_NUMPAD7;
    if (k == "NUM+8" || k == "NUMPAD8") return VK_NUMPAD8;
    if (k == "NUM+9" || k == "NUMPAD9") return VK_NUMPAD9;
    if (k == "NUM+*" || k == "MULTIPLY")  return VK_MULTIPLY;
    if (k == "NUM++" || k == "ADD")       return VK_ADD;
    if (k == "NUM+-" || k == "SUBTRACT")  return VK_SUBTRACT;
    if (k == "NUM+." || k == "DECIMAL")   return VK_DECIMAL;
    if (k == "NUM+/" || k == "DIVIDE")    return VK_DIVIDE;
    if (k == "NUM+ENTER")                 return VK_RETURN; 


    

    if (k == "-")   return VK_OEM_MINUS;
    if (k == "=")   return VK_OEM_PLUS;
    if (k == "[")   return VK_OEM_4;
    if (k == "]")   return VK_OEM_6;
    if (k == ";")   return VK_OEM_1;
    if (k == "'")   return VK_OEM_7;
    if (k == ",")   return VK_OEM_COMMA;
    if (k == ".")   return VK_OEM_PERIOD;
    if (k == "/")   return VK_OEM_2;
    if (k == "\\") return VK_OEM_5;
    if (k == "`")   return VK_OEM_3;

    

    if (k.length() == 1) {
        char c = k.at(0).toLatin1();
        if (c >= 'A' && c <= 'Z') return (WORD)c;
        if (c >= '0' && c <= '9') return (WORD)c;
    }
    return 0;
}


static bool isMouseVK(WORD vk) {
    return vk == VK_LBUTTON || vk == VK_RBUTTON || vk == VK_MBUTTON
           || vk == VK_XBUTTON1 || vk == VK_XBUTTON2;
}


static void sendMouseDown(WORD vk) {
    INPUT inp = {}; inp.type = INPUT_MOUSE;
    if (vk == VK_LBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    if (vk == VK_RBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    if (vk == VK_MBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
    if (vk == VK_XBUTTON1) { inp.mi.dwFlags = MOUSEEVENTF_XDOWN; inp.mi.mouseData = XBUTTON1; }
    if (vk == VK_XBUTTON2) { inp.mi.dwFlags = MOUSEEVENTF_XDOWN; inp.mi.mouseData = XBUTTON2; }
    if (inp.mi.dwFlags) SendInput(1, &inp, sizeof(INPUT));
}


static void sendMouseUp(WORD vk) {
    INPUT inp = {}; inp.type = INPUT_MOUSE;
    if (vk == VK_LBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    if (vk == VK_RBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    if (vk == VK_MBUTTON)  inp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
    if (vk == VK_XBUTTON1) { inp.mi.dwFlags = MOUSEEVENTF_XUP; inp.mi.mouseData = XBUTTON1; }
    if (vk == VK_XBUTTON2) { inp.mi.dwFlags = MOUSEEVENTF_XUP; inp.mi.mouseData = XBUTTON2; }
    if (inp.mi.dwFlags) SendInput(1, &inp, sizeof(INPUT));
}


static void sendKeyTap(WORD vk) {
    if (!vk) return;
    if (isMouseVK(vk)) {
        sendMouseDown(vk);
        Sleep(10);
        sendMouseUp(vk);
    } else {
        INPUT inp[2] = {};
        inp[0].type = INPUT_KEYBOARD; inp[0].ki.wVk = vk;
        inp[1].type = INPUT_KEYBOARD; inp[1].ki.wVk = vk; inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inp, sizeof(INPUT));
    }
}


static void sendKeyDown(WORD vk) {
    if (!vk || isMouseVK(vk)) return;
    INPUT inp = {};
    inp.type = INPUT_KEYBOARD; inp.ki.wVk = vk;
    SendInput(1, &inp, sizeof(INPUT));
}


static void sendKeyUp(WORD vk) {
    if (!vk || isMouseVK(vk)) return;
    INPUT inp = {};
    inp.type = INPUT_KEYBOARD; inp.ki.wVk = vk; inp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));
}


static bool isFortniteActive() {
    HWND hw = GetForegroundWindow();
    if (!hw) return false;
    WCHAR title[256] = {};
    GetWindowTextW(hw, title, 256);
    return QString::fromWCharArray(title).toLower().contains("fortnite");
}


class DragMacroWorker : public QObject {
    Q_OBJECT
public:
    

    

    DragMacroWorker(WORD editVk, int , QObject* p = nullptr)
        : QObject(p), m_editVk(editVk) {}
    void stop() { m_running = 0; }
public slots:
    void run() {
        m_running = 1;
        sendKeyDown(m_editVk);        

        sendMouseDown(VK_LBUTTON);    

        

        while (m_running.loadRelaxed()) {
            Sleep(5);
        }
        sendMouseUp(VK_LBUTTON);      

        sendKeyUp(m_editVk);          

        emit finished();
    }
signals:
    void finished();
private:
    WORD       m_editVk;
    QAtomicInt m_running{0};
};


class DoubleEditWorker : public QObject {
    Q_OBJECT
public:
    DoubleEditWorker(WORD editVk, WORD selectVk, int delayMs, QObject* p = nullptr)
        : QObject(p), m_editVk(editVk), m_selectVk(selectVk), m_delay(delayMs) {}
public slots:
    void run() {
        sendKeyTap(m_editVk);
        Sleep((DWORD)m_delay);
        sendKeyTap(m_selectVk);
        emit finished();
    }
signals:
    void finished();
private:
    WORD m_editVk, m_selectVk;
    int  m_delay;
};


class AutoBuildWorker : public QObject {
    Q_OBJECT
public:
    explicit AutoBuildWorker(WORD placeVk, QObject* p = nullptr)
        : QObject(p), m_placeVk(placeVk) {}
    void stop() { m_running = 0; }
public slots:
    void run() {
        m_running = 1;
        if (isMouseVK(m_placeVk))
            sendMouseDown(m_placeVk);
        else
            sendKeyDown(m_placeVk);
        

        while (m_running.loadRelaxed()) {
            Sleep(5);
        }
        if (isMouseVK(m_placeVk))
            sendMouseUp(m_placeVk);
        else
            sendKeyUp(m_placeVk);
        emit finished();
    }
signals:
    void finished();
private:
    WORD       m_placeVk;
    QAtomicInt m_running{0};
};


class HotkeyPoller : public QObject {
    Q_OBJECT
public:
    explicit HotkeyPoller(QObject* parent = nullptr) : QObject(parent) {}

    void setWatchKey(const QString& name, WORD vk) {
        QMutexLocker lk(&m_mutex);
        if (vk) m_keys[name] = vk;
        else    m_keys.remove(name);
    }

    void stop() { m_running = 0; }

public slots:
    void run() {
        m_running = 1;
        QMap<WORD, bool> wasDown;
        while (m_running.loadRelaxed()) {
            QMap<QString, WORD> snap;
            { QMutexLocker lk(&m_mutex); snap = m_keys; }
            QSet<WORD> seen;
            for (auto it = snap.constBegin(); it != snap.constEnd(); ++it) {
                WORD vk = it.value();
                if (!vk || seen.contains(vk)) continue;
                seen.insert(vk);
                bool down = pollKey(vk);
                if ( down && !wasDown[vk]) emit keyPressed(vk);
                if (!down &&  wasDown[vk]) emit keyReleased(vk);
                wasDown[vk] = down;
            }
            Sleep(10);
        }
        emit stopped();
    }

    

    

    static bool pollKey(WORD vk) {
        if (vk == VK_XBUTTON1 || vk == VK_XBUTTON2) {
            

            return (GetKeyState(vk) & 0x8000) != 0;
        }
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }
signals:
    void keyPressed(WORD vk);
    void keyReleased(WORD vk);
    void stopped();
private:
    QAtomicInt          m_running{0};
    QMutex              m_mutex;
    QMap<QString, WORD> m_keys;

};


class DashboardPage : public QWidget {
    Q_OBJECT
public:
    ~DashboardPage() {
        

        stopDragMacro();
        stopAutoBuildMacro();
        if (m_poller) { m_poller->stop(); }
        if (m_pollThread) { m_pollThread->quit(); m_pollThread->wait(2000); }
    }

    explicit DashboardPage(QWidget* parent = nullptr) : QWidget(parent) {
        setStyleSheet(inputCSS());
        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(18, 8, 18, 8);
        root->setSpacing(8);

        

        auto* cardsRow = new QHBoxLayout; cardsRow->setSpacing(12);
        const char* names[4] = {"Drag Macro","Double Edit","Pickaxe/Shotgun","Auto Build"};
        MacroCard::IconType icons[4] = {
            MacroCard::Arrow, MacroCard::Bolt, MacroCard::Crosshair, MacroCard::Grid
        };
        for (int i = 0; i < 4; i++) {
            m_cards[i] = new MacroCard(names[i], icons[i], i, this);
            connect(m_cards[i], &MacroCard::clicked, this, &DashboardPage::onCardClicked);
            cardsRow->addWidget(m_cards[i]);
        }
        root->addLayout(cardsRow);

        

        m_contentArea = new QWidget(this);
        m_contentArea->setStyleSheet("background:transparent;");
        auto* contentLay = new QVBoxLayout(m_contentArea);
        contentLay->setContentsMargins(0, 0, 0, 0);
        contentLay->setSpacing(8);

        

        m_contentEffect = new QGraphicsOpacityEffect(m_contentArea);
        m_contentEffect->setOpacity(1.0);
        m_contentArea->setGraphicsEffect(m_contentEffect);

        

        auto* infoRow = new QHBoxLayout; infoRow->setSpacing(8);
        m_infoMode = new InfoBox("ACTIVE MODE", m_contentArea);
        m_infoMode->setValue("—");
        infoRow->addWidget(m_infoMode, 1);

        auto* statusBox = new InfoBox("STATUS", m_contentArea);
        statusBox->valueLabel()->hide();
        auto* sbLay = qobject_cast<QVBoxLayout*>(statusBox->layout());
        if (sbLay) {
            auto* dotRow = new QHBoxLayout; dotRow->setSpacing(6);
            m_statusDot = new StatusDot(statusBox);
            m_statusTxt = new QLabel("INACTIVE", statusBox);
            m_statusTxt->setObjectName("statusTxt");
            m_statusTxt->setStyleSheet("#statusTxt { color:#FFFFFF; font-size:13px; font-weight:600; background:transparent; }");
            dotRow->addWidget(m_statusDot);
            dotRow->addWidget(m_statusTxt);
            dotRow->addStretch();
            sbLay->addLayout(dotRow);
        }
        infoRow->addWidget(statusBox, 1);
        contentLay->addLayout(infoRow);

        

        m_settingsStack = new AnimatedStack(m_contentArea);
        m_settingsStack->setStyleSheet("QStackedWidget{background:transparent;}");

        

        auto* placeholder = new Panel;
        auto* phLay = new QVBoxLayout(placeholder);
        phLay->setContentsMargins(14, 0, 14, 0);
        phLay->setAlignment(Qt::AlignCenter);
        auto* phTxt = new QLabel("Select a macro above to configure it");
        phTxt->setStyleSheet("color:#444444;font-size:12px;font-style:italic;background:transparent;");
        phTxt->setAlignment(Qt::AlignCenter);
        phLay->addWidget(phTxt);
        m_settingsStack->addWidget(placeholder);   


        

        auto mkBindRow = [](QWidget* parent, const QString& name,
                            const QString& sub, const QString& key) -> QWidget* {
            auto* row = new QWidget(parent);
            row->setFixedHeight(46);
            row->setStyleSheet("background:transparent;");
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(4, 0, 4, 0); rl->setSpacing(0);
            auto* txt = new QVBoxLayout; txt->setSpacing(1);
            auto* lbl = new QLabel(name);
            lbl->setStyleSheet("color:#F0F0F0;font-size:13px;background:transparent;");
            auto* sub2 = new QLabel(sub);
            sub2->setStyleSheet("color:#606060;font-size:10px;background:transparent;");
            txt->addWidget(lbl); txt->addWidget(sub2);
            rl->addLayout(txt, 1);
            rl->addWidget(new KeyBindButton(key, parent));
            return row;
        };

        auto mkToggleBindRow = [](QWidget* parent, const QString& name,
                                  const QString& sub, const QString& key) -> QWidget* {
            auto* row = new QWidget(parent);
            row->setFixedHeight(46);
            row->setStyleSheet("background:transparent;");
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(4, 0, 4, 0); rl->setSpacing(10);
            auto* txt = new QVBoxLayout; txt->setSpacing(1);
            auto* lbl = new QLabel(name);
            lbl->setStyleSheet("color:#F0F0F0;font-size:13px;background:transparent;");
            auto* sub2 = new QLabel(sub);
            sub2->setStyleSheet("color:#606060;font-size:10px;background:transparent;");
            txt->addWidget(lbl); txt->addWidget(sub2);
            rl->addLayout(txt, 1);
            rl->addWidget(new ToggleSwitch(parent));
            rl->addWidget(new KeyBindButton(key, parent));
            return row;
        };

        auto mkSpinRow = [](QWidget* parent, const QString& name,
                            const QString& sub, int val, int lo, int hi) -> QWidget* {
            auto* row = new QWidget(parent);
            row->setFixedHeight(46);
            row->setStyleSheet("background:transparent;");
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(4, 0, 4, 0); rl->setSpacing(0);
            auto* txt = new QVBoxLayout; txt->setSpacing(1);
            auto* lbl = new QLabel(name);
            lbl->setStyleSheet("color:#F0F0F0;font-size:13px;background:transparent;");
            auto* sub2 = new QLabel(sub);
            sub2->setStyleSheet("color:#606060;font-size:10px;background:transparent;");
            txt->addWidget(lbl); txt->addWidget(sub2);
            rl->addLayout(txt, 1);
            auto* sb = new QSpinBox; sb->setRange(lo, hi); sb->setValue(val);
            sb->setFixedSize(90, 30); rl->addWidget(sb);
            return row;
        };

        

        auto mkPanel = [&](const QString& title, int idx,
                           std::function<void(QWidget*, QVBoxLayout*)> buildRows)
            -> QPair<QWidget*, ToggleSwitch*> {
            auto* p = new Panel;
            auto* vl = new QVBoxLayout(p);
            vl->setContentsMargins(16, 8, 16, 8); vl->setSpacing(0);

            

            auto* hdr = new QWidget(p); hdr->setStyleSheet("background:transparent;");
            hdr->setFixedHeight(42);
            auto* hl = new QHBoxLayout(hdr);
            hl->setContentsMargins(4, 4, 4, 4); hl->setSpacing(8);
            auto* titleLbl = new QLabel(title);
            titleLbl->setStyleSheet(
                "color:#F0F0F0;font-size:12px;font-weight:600;"
                "border-left:3px solid #FFFFFF;padding-left:7px;background:transparent;");
            hl->addWidget(titleLbl, 1);
            auto* enLbl = new QLabel("Enabled");
            enLbl->setStyleSheet("color:#888888;font-size:11px;background:transparent;");
            hl->addWidget(enLbl);
            auto* tog = new ToggleSwitch(p);
            tog->setOn(false);   

            hl->addWidget(tog);
            vl->addWidget(hdr);
            vl->addWidget(hline());
            vl->addSpacing(6);

            buildRows(p, vl);
            vl->addStretch();
            Q_UNUSED(idx)
            return {p, tog};
        };

        

        KeyBindButton* dragEditBtn   = nullptr;

        

        {
            auto [p, tog] = mkPanel("Drag Macro", 1, [&](QWidget* p, QVBoxLayout* vl){
                auto* editRow   = mkBindRow(p, "Edit bind",   "key you use to edit",   "F");
                auto* toggleRow = mkBindRow(p, "Toggle bind", "hold to drag-select",   "E");
                auto* ehl = qobject_cast<QHBoxLayout*>(editRow->layout());
                auto* thl = qobject_cast<QHBoxLayout*>(toggleRow->layout());
                if (ehl) dragEditBtn = qobject_cast<KeyBindButton*>(ehl->itemAt(ehl->count()-1)->widget());
                auto* dragToggleBtn  = thl ? qobject_cast<KeyBindButton*>(thl->itemAt(thl->count()-1)->widget()) : nullptr;
                m_dragEditBtn   = dragEditBtn;
                m_dragToggleBtn = dragToggleBtn;
                if (m_dragToggleBtn)
                    connect(m_dragToggleBtn, &KeyBindButton::keyChanged,
                            this, [this](const QString&){ refreshDragPollerKey(); });
                

                auto* info = new QLabel("Hold Toggle bind to hold Edit + LMB.\nRelease to stop.", p);
                info->setStyleSheet("color:#888;font-size:10px;background:transparent;padding:4px 0;");
                info->setWordWrap(true);
                vl->addWidget(editRow);   vl->addWidget(hline());
                vl->addWidget(toggleRow); vl->addWidget(hline());
                vl->addWidget(info);
            });
            m_macroToggles[0] = tog;
            connect(tog, &ToggleSwitch::toggled, this, [this](bool on){ onMacroToggled(0, on); });
            m_settingsStack->addWidget(p);   

        }

        

        {
            auto [p, tog] = mkPanel("Double Edit", 2, [&](QWidget* p, QVBoxLayout* vl){
                auto* editRow   = mkBindRow(p, "Edit bind",   "key you use to edit",      "F");
                auto* selRow    = mkBindRow(p, "Select bind", "key that confirms edit",    "O");
                auto* toggleRow = mkBindRow(p, "Toggle bind", "press to double edit",      "C");
                auto* delayRow  = mkSpinRow(p, "Delay (ms)",  "gap between Edit & Select", 80, 1, 999);
                auto* ehl = qobject_cast<QHBoxLayout*>(editRow->layout());
                auto* shl = qobject_cast<QHBoxLayout*>(selRow->layout());
                auto* thl = qobject_cast<QHBoxLayout*>(toggleRow->layout());
                auto* dhl = qobject_cast<QHBoxLayout*>(delayRow->layout());
                auto* deEditBtn   = ehl ? qobject_cast<KeyBindButton*>(ehl->itemAt(ehl->count()-1)->widget()) : nullptr;
                auto* deSelBtn    = shl ? qobject_cast<KeyBindButton*>(shl->itemAt(shl->count()-1)->widget()) : nullptr;
                auto* deToggleBtn = thl ? qobject_cast<KeyBindButton*>(thl->itemAt(thl->count()-1)->widget()) : nullptr;
                if (dhl) m_deDelayBox = qobject_cast<QSpinBox*>(dhl->itemAt(dhl->count()-1)->widget());
                if (dragEditBtn && deEditBtn) {
                    connect(dragEditBtn, &KeyBindButton::keyChanged, deEditBtn,   &KeyBindButton::setKeySilent);
                    connect(deEditBtn,   &KeyBindButton::keyChanged, dragEditBtn, &KeyBindButton::setKeySilent);
                }
                m_deEditBtn   = deEditBtn;
                m_deSelectBtn = deSelBtn;
                m_deToggleBtn = deToggleBtn;
                if (m_deToggleBtn)
                    connect(m_deToggleBtn, &KeyBindButton::keyChanged,
                            this, [this](const QString&){ refreshDoubleEditPollerKey(); });
                vl->addWidget(editRow);   vl->addWidget(hline());
                vl->addWidget(selRow);    vl->addWidget(hline());
                vl->addWidget(toggleRow); vl->addWidget(hline());
                vl->addWidget(delayRow);
            });
            m_macroToggles[1] = tog;
            connect(tog, &ToggleSwitch::toggled, this, [this](bool on){ onMacroToggled(1, on); });
            m_settingsStack->addWidget(p);   

        }

        

        {
            auto [p, tog] = mkPanel("Pickaxe/Shotgun", 3, [&](QWidget* p, QVBoxLayout* vl){
                auto* macroRow = mkBindRow(p, "Toggle bind", "hold to enable shotgun pullout", "-");
                auto* slotRow  = mkBindRow(p, "Slot bind",   "slot key pressed after LMB release", "1");
                auto* delayRow = mkSpinRow(p, "Cooldown (ms)", "re-fire delay after tap", 300, 50, 2000);
                auto* mhl = qobject_cast<QHBoxLayout*>(macroRow->layout());
                auto* shl = qobject_cast<QHBoxLayout*>(slotRow->layout());
                auto* dhl = qobject_cast<QHBoxLayout*>(delayRow->layout());
                if (mhl) m_shotgunMacroBtn = qobject_cast<KeyBindButton*>(mhl->itemAt(mhl->count()-1)->widget());
                if (shl) m_shotgunSlotBtn  = qobject_cast<KeyBindButton*>(shl->itemAt(shl->count()-1)->widget());
                if (dhl) m_shotgunDelayBox = qobject_cast<QSpinBox*>(dhl->itemAt(dhl->count()-1)->widget());
                if (m_shotgunMacroBtn)
                    connect(m_shotgunMacroBtn, &KeyBindButton::keyChanged,
                            this, [this](const QString&){ refreshShotgunPollerKey(); });
                

                auto* info = new QLabel("Hold Toggle bind, then left click.\nSlot bind fires on LMB release.", p);
                info->setStyleSheet("color:#888;font-size:10px;background:transparent;padding:4px 0;");
                info->setWordWrap(true);
                vl->addWidget(macroRow); vl->addWidget(hline());
                vl->addWidget(slotRow);  vl->addWidget(hline());
                vl->addWidget(delayRow); vl->addWidget(hline());
                vl->addWidget(info);
            });
            m_macroToggles[2] = tog;
            connect(tog, &ToggleSwitch::toggled, this, [this](bool on){ onMacroToggled(2, on); });
            m_settingsStack->addWidget(p);   

        }

        

        {
            auto [p, tog] = mkPanel("Auto Build", 4, [&](QWidget* p, QVBoxLayout* vl){
                

                auto* wallRow  = mkBindRow(p, "Wall bind",   "your wall key",         "MB4");
                auto* floorRow = mkBindRow(p, "Floor bind",  "your floor key",        "MB5");
                auto* stairRow = mkBindRow(p, "Stair bind",  "your stair key",        "C");
                auto* coneRow  = mkBindRow(p, "Cone bind",   "your cone key",         "V");
                auto* placeRow = mkBindRow(p, "Place bind",  "key that places build", "MB1");
                std::function<KeyBindButton*(QWidget*)> getBtn = [](QWidget* row) -> KeyBindButton* {
                    auto* hl = qobject_cast<QHBoxLayout*>(row->layout());
                    return hl ? qobject_cast<KeyBindButton*>(hl->itemAt(hl->count()-1)->widget()) : nullptr;
                };
                m_abWallBtn  = getBtn(wallRow);
                m_abFloorBtn = getBtn(floorRow);
                m_abStairBtn = getBtn(stairRow);
                m_abConeBtn  = getBtn(coneRow);
                m_abPlaceBtn = getBtn(placeRow);
                auto refreshAB = [this]{ refreshAutoBuildPollerKeys(); };
                if (m_abWallBtn)  connect(m_abWallBtn,  &KeyBindButton::keyChanged, this, refreshAB);
                if (m_abFloorBtn) connect(m_abFloorBtn, &KeyBindButton::keyChanged, this, refreshAB);
                if (m_abStairBtn) connect(m_abStairBtn, &KeyBindButton::keyChanged, this, refreshAB);
                if (m_abConeBtn)  connect(m_abConeBtn,  &KeyBindButton::keyChanged, this, refreshAB);
                vl->addWidget(wallRow);  vl->addWidget(hline());
                vl->addWidget(floorRow); vl->addWidget(hline());
                vl->addWidget(stairRow); vl->addWidget(hline());
                vl->addWidget(coneRow);  vl->addWidget(hline());
                vl->addWidget(placeRow);
            });
            m_macroToggles[3] = tog;
            connect(tog, &ToggleSwitch::toggled, this, [this](bool on){ onMacroToggled(3, on); });
            m_settingsStack->addWidget(p);   

        }

        contentLay->addWidget(m_settingsStack, 1);
        root->addWidget(m_contentArea, 1);

        

        updateStatus(false);

        

        loadSettings();

        

        auto save = [this](){ saveSettings(); };
        for (auto* btn : {m_dragEditBtn, m_dragToggleBtn,
                          m_deEditBtn, m_deSelectBtn, m_deToggleBtn,
                          m_shotgunMacroBtn, m_shotgunSlotBtn,
                          m_abWallBtn, m_abFloorBtn, m_abStairBtn, m_abConeBtn, m_abPlaceBtn})
            if (btn) connect(btn, &KeyBindButton::keyChanged, this, save);
        for (auto* sb : {m_dragDelayBox, m_deDelayBox, m_shotgunDelayBox})
            if (sb) connect(sb, &QSpinBox::valueChanged, this, save);
        for (auto* tog : m_macroToggles)
            if (tog) connect(tog, &ToggleSwitch::toggled, this, save);
    }

    

    

    void setRunning(bool r) {
        

        if (m_currentMacro >= 0) {
            bool macroOn = m_macroToggles[m_currentMacro]->isOn();
            updateStatus(macroOn);
        } else {
            updateStatus(false);
        }
        Q_UNUSED(r)
    }

    

    void animateContentSwitch(std::function<void()> swapFn) {
        if (!m_contentEffect) { swapFn(); return; }

        

        

        if (m_contentFadeOut) { m_contentFadeOut->stop(); m_contentFadeOut->deleteLater(); m_contentFadeOut = nullptr; }
        if (m_contentFadeIn)  { m_contentFadeIn->stop();  m_contentFadeIn->deleteLater();  m_contentFadeIn  = nullptr; }

        

        m_contentFadeOut = new QPropertyAnimation(m_contentEffect, "opacity", this);
        m_contentFadeOut->setDuration(110);
        m_contentFadeOut->setStartValue(m_contentEffect->opacity());
        m_contentFadeOut->setEndValue(0.0);
        m_contentFadeOut->setEasingCurve(QEasingCurve::OutCubic);
        connect(m_contentFadeOut, &QPropertyAnimation::finished, this, [this, swapFn]{
            

            swapFn();
            

            m_contentFadeIn = new QPropertyAnimation(m_contentEffect, "opacity", this);
            m_contentFadeIn->setDuration(180);
            m_contentFadeIn->setStartValue(0.0);
            m_contentFadeIn->setEndValue(1.0);
            m_contentFadeIn->setEasingCurve(QEasingCurve::OutCubic);
            

            connect(m_contentFadeIn, &QPropertyAnimation::finished, this, [this]{
                m_contentEffect->setOpacity(1.0);
                m_contentFadeIn = nullptr;
            });
            m_contentFadeIn->start();
            m_contentFadeOut = nullptr;
        });
        m_contentFadeOut->start();
    }

private slots:
    void onCardClicked(int id) {
        bool wasSelected = m_cards[id]->isSelected();
        for (int i = 0; i < 4; i++) m_cards[i]->setSelected(i == id && !wasSelected);
        animateContentSwitch([this, id, wasSelected]{
            if (wasSelected) {
                m_currentMacro = -1;
                m_infoMode->setValue("—");
                m_settingsStack->switchTo(0);
                updateStatus(false);
            } else {
                m_currentMacro = id;
                const char* n[4] = {"Drag Macro","Double Edit","Pickaxe/Shotgun","Auto Build"};
                m_infoMode->setValue(n[id]);
                m_settingsStack->switchTo(id + 1);
                updateStatus(m_macroToggles[id]->isOn());
            }
        });
    }

    void updateStatus(bool on) {
        if (!m_statusDot || !m_statusTxt) return;
        m_statusDot->setActive(on);
        m_statusTxt->setText(on ? "ACTIVE" : "INACTIVE");
    }
    void onMacroToggled(int id, bool on) {
        if (m_loading) return;
        if (m_currentMacro == id) updateStatus(on);

        if (id == 0) {  

            if (on) {
                startHotkeyPoller();
                refreshDragPollerKey();
            } else {
                stopDragMacro();   

                if (m_poller) m_poller->setWatchKey("drag", 0);
                checkStopPoller();
            }
        }
        if (id == 1) {  

            if (on) {
                startHotkeyPoller();
                refreshDoubleEditPollerKey();
            } else {
                m_deRunning = false;  

                if (m_poller) m_poller->setWatchKey("de", 0);
                checkStopPoller();
            }
        }
        if (id == 2) {  

            if (on) {
                startHotkeyPoller();
                refreshShotgunPollerKey();
            } else {
                m_sgCooldown = false;
                m_sgArmed    = false;
                if (m_poller) {
                    m_poller->setWatchKey("sg",        0);
                    m_poller->setWatchKey("sg_toggle", 0);
                }
                checkStopPoller();
            }
        }
        if (id == 3) {  

            if (on) {
                startHotkeyPoller();
                refreshAutoBuildPollerKeys();
            } else {
                stopAutoBuildMacro();
                if (m_poller) {
                    m_poller->setWatchKey("ab_wall",  0);
                    m_poller->setWatchKey("ab_floor", 0);
                    m_poller->setWatchKey("ab_stair", 0);
                    m_poller->setWatchKey("ab_cone",  0);
                    m_poller->setWatchKey("ab_place", 0);
                }
                checkStopPoller();
            }
        }
    }

    void onHotkeyPressed(WORD vk) {
        

        


        

        if (m_macroToggles[0] && m_macroToggles[0]->isOn()) {
            WORD tv = keyNameToVK(m_dragToggleBtn ? m_dragToggleBtn->text() : "E");
            if (vk == tv && !m_dragWorker) {
                WORD ev = keyNameToVK(m_dragEditBtn ? m_dragEditBtn->text() : "F");
                int  delay = m_dragDelayBox ? m_dragDelayBox->value() : 80;
                if (ev) {
                    m_dragThread = new QThread(this);
                    m_dragWorker = new DragMacroWorker(ev, delay);
                    m_dragWorker->moveToThread(m_dragThread);
                    connect(m_dragThread, &QThread::started,  m_dragWorker, &DragMacroWorker::run);
                    connect(m_dragWorker, &DragMacroWorker::finished, m_dragThread, &QThread::quit);
                    connect(m_dragWorker, &DragMacroWorker::finished, m_dragWorker, &QObject::deleteLater);
                    connect(m_dragThread, &QThread::finished, m_dragThread, &QObject::deleteLater);
                    connect(m_dragWorker, &DragMacroWorker::finished, this, [this](){
                        m_dragWorker = nullptr; m_dragThread = nullptr;
                    });
                    m_dragThread->start();
                }
            }
        }

        

        

        if (m_macroToggles[1] && m_macroToggles[1]->isOn() && !m_dragWorker) {
            WORD tv = keyNameToVK(m_deToggleBtn ? m_deToggleBtn->text() : "C");
            if (vk == tv && !m_deRunning) {
                WORD ev    = keyNameToVK(m_deEditBtn   ? m_deEditBtn->text()   : "F");
                WORD sv    = keyNameToVK(m_deSelectBtn ? m_deSelectBtn->text() : "O");
                int  delay = m_deDelayBox ? m_deDelayBox->value() : 80;
                if (ev && sv) {
                    m_deRunning = true;
                    auto* t = new QThread(this);
                    auto* w = new DoubleEditWorker(ev, sv, delay);
                    w->moveToThread(t);
                    connect(t, &QThread::started, w, &DoubleEditWorker::run);
                    connect(w, &DoubleEditWorker::finished, t, &QThread::quit);
                    connect(w, &DoubleEditWorker::finished, w, &QObject::deleteLater);
                    connect(t, &QThread::finished, t, &QObject::deleteLater);
                    connect(w, &DoubleEditWorker::finished, this, [this](){ m_deRunning = false; });
                    t->start();
                }
            }
        }

        

        

        if (m_macroToggles[2] && m_macroToggles[2]->isOn()) {
            WORD tv = keyNameToVK(m_shotgunMacroBtn ? m_shotgunMacroBtn->text() : "-");
            bool toggleHeld = tv && HotkeyPoller::pollKey(tv);
            if (vk == VK_LBUTTON && toggleHeld && !m_sgCooldown) {
                m_sgArmed = true;   

            }
        }

        

        if (m_macroToggles[3] && m_macroToggles[3]->isOn()) {
            WORD placeVk = keyNameToVK(m_abPlaceBtn ? m_abPlaceBtn->text() : "MB1");
            if (placeVk && !m_abWorker) {
                KeyBindButton* typeBtns[] = {m_abWallBtn, m_abFloorBtn, m_abStairBtn, m_abConeBtn};
                for (auto* btn : typeBtns) {
                    if (!btn) continue;
                    WORD bv = keyNameToVK(btn->text());
                    if (vk == bv) {
                        

                        m_abActiveVk = bv;
                        m_abThread   = new QThread(this);
                        m_abWorker   = new AutoBuildWorker(placeVk);
                        m_abWorker->moveToThread(m_abThread);
                        connect(m_abThread, &QThread::started,    m_abWorker, &AutoBuildWorker::run);
                        connect(m_abWorker, &AutoBuildWorker::finished, m_abThread, &QThread::quit);
                        connect(m_abWorker, &AutoBuildWorker::finished, m_abWorker, &QObject::deleteLater);
                        connect(m_abThread, &QThread::finished,   m_abThread, &QObject::deleteLater);
                        connect(m_abWorker, &AutoBuildWorker::finished, this, [this](){
                            m_abWorker   = nullptr;
                            m_abThread   = nullptr;
                            m_abActiveVk = 0;
                        });
                        m_abThread->start();
                        break;
                    }
                }
            }
        }
    }

    void onHotkeyReleased(WORD vk) {
        

        WORD tv = keyNameToVK(m_dragToggleBtn ? m_dragToggleBtn->text() : "E");
        if (vk == tv) stopDragMacro();

        

        if (m_abWorker && vk == m_abActiveVk) {
            m_abWorker->stop();
        }

        


        

        if (m_macroToggles[2] && m_macroToggles[2]->isOn()) {
            if (vk == VK_LBUTTON && m_sgArmed && !m_sgCooldown) {
                m_sgArmed = false;
                WORD slot     = keyNameToVK(m_shotgunSlotBtn ? m_shotgunSlotBtn->text() : "1");
                int  cooldown = m_shotgunDelayBox ? m_shotgunDelayBox->value() : 300;
                if (slot) {
                    Sleep(1);           

                    sendKeyTap(slot);
                    m_sgCooldown = true;
                    QTimer::singleShot(cooldown, this, [this]{ m_sgCooldown = false; });
                }
            } else if (vk == VK_LBUTTON) {
                m_sgArmed = false;      

            }
        }
    }

    


    void stopDragMacro() {
        if (m_dragWorker) { m_dragWorker->stop(); m_dragWorker = nullptr; m_dragThread = nullptr; }
    }

    void stopAutoBuildMacro() {
        if (m_abWorker) { m_abWorker->stop(); m_abWorker = nullptr; m_abThread = nullptr; m_abActiveVk = 0; }
    }

    void refreshDragPollerKey() {
        if (m_poller) m_poller->setWatchKey("drag", keyNameToVK(m_dragToggleBtn ? m_dragToggleBtn->text() : "E"));
    }
    void refreshDoubleEditPollerKey() {
        if (m_poller) m_poller->setWatchKey("de", keyNameToVK(m_deToggleBtn ? m_deToggleBtn->text() : "C"));
    }
    void refreshShotgunPollerKey() {
        if (!m_poller) return;
        

        m_poller->setWatchKey("sg", VK_LBUTTON);
        

        WORD tv = keyNameToVK(m_shotgunMacroBtn ? m_shotgunMacroBtn->text() : "-");
        m_poller->setWatchKey("sg_toggle", tv);
    }
    void refreshAutoBuildPollerKeys() {
        if (!m_poller) return;
        m_poller->setWatchKey("ab_wall",  keyNameToVK(m_abWallBtn  ? m_abWallBtn->text()  : "MB4"));
        m_poller->setWatchKey("ab_floor", keyNameToVK(m_abFloorBtn ? m_abFloorBtn->text() : "MB5"));
        m_poller->setWatchKey("ab_stair", keyNameToVK(m_abStairBtn ? m_abStairBtn->text() : "C"));
        m_poller->setWatchKey("ab_cone",  keyNameToVK(m_abConeBtn  ? m_abConeBtn->text()  : "V"));
        

        m_poller->setWatchKey("ab_place", keyNameToVK(m_abPlaceBtn ? m_abPlaceBtn->text() : "MB1"));
    }

    void checkStopPoller() {
        bool anyOn = false;
        for (auto* t : m_macroToggles) if (t && t->isOn()) { anyOn = true; break; }
        if (!anyOn) stopHotkeyPoller();
    }

    void startHotkeyPoller() {
        if (m_pollThread) return;
        m_pollThread = new QThread(this);
        m_poller     = new HotkeyPoller;
        m_poller->moveToThread(m_pollThread);
        connect(m_pollThread, &QThread::started,  m_poller, &HotkeyPoller::run);
        connect(m_poller, &HotkeyPoller::stopped, m_pollThread, &QThread::quit);
        connect(m_poller, &HotkeyPoller::stopped, m_poller,     &QObject::deleteLater);
        connect(m_pollThread, &QThread::finished, m_pollThread, &QObject::deleteLater);
        connect(m_poller, &HotkeyPoller::keyPressed,  this, &DashboardPage::onHotkeyPressed);
        connect(m_poller, &HotkeyPoller::keyReleased, this, &DashboardPage::onHotkeyReleased);
        m_pollThread->start();
    }


    


    QString settingsPath() const {
        return QCoreApplication::applicationDirPath() + "/settings.json";
    }

public:
    void stopHotkeyPoller() {
        if (m_poller) { m_poller->stop(); m_poller = nullptr; }
        if (m_pollThread) { m_pollThread->quit(); m_pollThread->wait(2000); m_pollThread = nullptr; }
    }

    void saveSettings() {
        if (m_loading) return;
        QJsonObject o;
        o["drag_enabled"]   = m_macroToggles[0] ? m_macroToggles[0]->isOn() : false;
        o["de_enabled"]     = m_macroToggles[1] ? m_macroToggles[1]->isOn() : false;
        o["sg_enabled"]     = m_macroToggles[2] ? m_macroToggles[2]->isOn() : false;
        o["ab_enabled"]     = m_macroToggles[3] ? m_macroToggles[3]->isOn() : false;
        o["drag_edit"]      = m_dragEditBtn    ? m_dragEditBtn->text()    : "F";
        o["drag_toggle"]    = m_dragToggleBtn  ? m_dragToggleBtn->text()  : "E";
        o["drag_delay"]     = m_dragDelayBox   ? m_dragDelayBox->value()  : 80;
        o["de_edit"]        = m_deEditBtn      ? m_deEditBtn->text()      : "F";
        o["de_select"]      = m_deSelectBtn    ? m_deSelectBtn->text()    : "O";
        o["de_toggle"]      = m_deToggleBtn    ? m_deToggleBtn->text()    : "C";
        o["de_delay"]       = m_deDelayBox     ? m_deDelayBox->value()    : 80;
        o["sg_toggle"]      = m_shotgunMacroBtn ? m_shotgunMacroBtn->text() : "-";
        o["sg_slot"]        = m_shotgunSlotBtn  ? m_shotgunSlotBtn->text()  : "1";
        o["sg_delay"]       = m_shotgunDelayBox ? m_shotgunDelayBox->value(): 300;
        o["ab_wall"]        = m_abWallBtn  ? m_abWallBtn->text()  : "MB4";
        o["ab_floor"]       = m_abFloorBtn ? m_abFloorBtn->text() : "MB5";
        o["ab_stair"]       = m_abStairBtn ? m_abStairBtn->text() : "C";
        o["ab_cone"]        = m_abConeBtn  ? m_abConeBtn->text()  : "V";
        o["ab_place"]       = m_abPlaceBtn ? m_abPlaceBtn->text() : "MB1";
        QFile f(settingsPath());
        if (f.open(QIODevice::WriteOnly)) f.write(QJsonDocument(o).toJson());
    }

private:
    void loadSettings() {
        QFile f(settingsPath());
        if (!f.open(QIODevice::ReadOnly)) return;
        QJsonObject o = QJsonDocument::fromJson(f.readAll()).object();
        if (o.isEmpty()) return;
        m_loading = true;
        auto setBtn = [](KeyBindButton* btn, const QJsonObject& o, const QString& key, const QString& def){
            if (btn) btn->setKeySilent(o.value(key).toString(def));
        };
        setBtn(m_dragEditBtn,    o, "drag_edit",   "F");
        setBtn(m_dragToggleBtn,  o, "drag_toggle", "E");
        if (m_dragDelayBox)    m_dragDelayBox->setValue(o.value("drag_delay").toInt(80));
        setBtn(m_deEditBtn,      o, "de_edit",     "F");
        setBtn(m_deSelectBtn,    o, "de_select",   "O");
        setBtn(m_deToggleBtn,    o, "de_toggle",   "C");
        if (m_deDelayBox)      m_deDelayBox->setValue(o.value("de_delay").toInt(80));
        setBtn(m_shotgunMacroBtn, o, "sg_toggle", "-");
        setBtn(m_shotgunSlotBtn,  o, "sg_slot",   "1");
        if (m_shotgunDelayBox) m_shotgunDelayBox->setValue(o.value("sg_delay").toInt(300));
        setBtn(m_abWallBtn,      o, "ab_wall",     "MB4");
        setBtn(m_abFloorBtn,     o, "ab_floor",    "MB5");
        setBtn(m_abStairBtn,     o, "ab_stair",    "C");
        setBtn(m_abConeBtn,      o, "ab_cone",     "V");
        setBtn(m_abPlaceBtn,     o, "ab_place",    "MB1");
        if (m_macroToggles[0]) m_macroToggles[0]->setOn(o.value("drag_enabled").toBool(false));
        if (m_macroToggles[1]) m_macroToggles[1]->setOn(o.value("de_enabled").toBool(false));
        if (m_macroToggles[2]) m_macroToggles[2]->setOn(o.value("sg_enabled").toBool(false));
        if (m_macroToggles[3]) m_macroToggles[3]->setOn(o.value("ab_enabled").toBool(false));
        m_loading = false;
        if (m_macroToggles[0] && m_macroToggles[0]->isOn()) { startHotkeyPoller(); refreshDragPollerKey(); }
        if (m_macroToggles[1] && m_macroToggles[1]->isOn()) { startHotkeyPoller(); refreshDoubleEditPollerKey(); }
        if (m_macroToggles[2] && m_macroToggles[2]->isOn()) { startHotkeyPoller(); refreshShotgunPollerKey(); }
        if (m_macroToggles[3] && m_macroToggles[3]->isOn()) { startHotkeyPoller(); refreshAutoBuildPollerKeys(); }
        if (m_currentMacro >= 0) updateStatus(m_macroToggles[m_currentMacro]->isOn());
    }

private:
    MacroCard*      m_cards[4]        = {};
    ToggleSwitch*   m_macroToggles[4] = {};
    int             m_currentMacro    = -1;
    InfoBox*        m_infoMode        = nullptr;
    StatusDot*      m_statusDot       = nullptr;
    QLabel*         m_statusTxt       = nullptr;
    AnimatedStack*          m_settingsStack   = nullptr;
    

    QWidget*                m_contentArea     = nullptr;
    QGraphicsOpacityEffect* m_contentEffect   = nullptr;
    QPropertyAnimation*     m_contentFadeOut  = nullptr;
    QPropertyAnimation*     m_contentFadeIn   = nullptr;

    bool            m_loading          = false;

    

    KeyBindButton*   m_dragEditBtn    = nullptr;
    KeyBindButton*   m_dragToggleBtn  = nullptr;
    QSpinBox*        m_dragDelayBox   = nullptr;
    DragMacroWorker* m_dragWorker     = nullptr;
    QThread*         m_dragThread     = nullptr;

    

    KeyBindButton*   m_deEditBtn      = nullptr;
    KeyBindButton*   m_deSelectBtn    = nullptr;
    KeyBindButton*   m_deToggleBtn    = nullptr;
    QSpinBox*        m_deDelayBox     = nullptr;
    bool             m_deRunning      = false;

    

    KeyBindButton*   m_shotgunMacroBtn = nullptr;
    KeyBindButton*   m_shotgunSlotBtn  = nullptr;
    QSpinBox*        m_shotgunDelayBox = nullptr;
    bool             m_sgCooldown      = false;
    bool             m_sgArmed         = false;   


    

    KeyBindButton*   m_abWallBtn      = nullptr;
    KeyBindButton*   m_abFloorBtn     = nullptr;
    KeyBindButton*   m_abStairBtn     = nullptr;
    KeyBindButton*   m_abConeBtn      = nullptr;
    KeyBindButton*   m_abPlaceBtn     = nullptr;
    AutoBuildWorker* m_abWorker       = nullptr;
    QThread*         m_abThread       = nullptr;
    WORD             m_abActiveVk     = 0;     


    

    HotkeyPoller*    m_poller         = nullptr;
    QThread*         m_pollThread     = nullptr;


};


class ToastNotification : public QWidget {
    Q_OBJECT
public:
    explicit ToastNotification(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(260, 44);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);

        

        m_animTimer = new QTimer(this);
        m_animTimer->setTimerType(Qt::PreciseTimer);
        m_animTimer->setInterval(8);
        connect(m_animTimer, &QTimer::timeout, this, &ToastNotification::onTick);

        

        m_holdTimer = new QTimer(this);
        m_holdTimer->setSingleShot(true);
        connect(m_holdTimer, &QTimer::timeout, this, [this]{
            m_phase = 2; 

            m_elapsed.restart();
            m_animTimer->start();
        });

        hide();
    }

    void popup(const QString& msg) {
        m_msg = msg;
        m_animTimer->stop();
        m_holdTimer->stop();
        m_opacity  = 1.0f;
        m_phase    = 0; 


        if (parentWidget()) {
            int px = (parentWidget()->width() - width()) / 2;
            m_startY = parentWidget()->height();
            m_endY   = parentWidget()->height() - height() - 16;
            m_yPos   = float(m_startY);
            move(px, m_startY);
            show(); raise();
        }
        m_elapsed.restart();
        m_animTimer->start();
        update();
    }

private slots:
    void onTick() {
        if (m_phase == 0) {
            

            const float duration = 220.0f;
            float progress = qBound(0.0f, float(m_elapsed.elapsed()) / duration, 1.0f);
            float ease = 1.0f - std::pow(1.0f - progress, 3.0f);
            m_yPos = float(m_startY) + ease * float(m_endY - m_startY);
            move(x(), int(m_yPos));
            update();
            if (progress >= 1.0f) {
                move(x(), m_endY);
                m_animTimer->stop();
                m_phase = 1;
                m_holdTimer->start(2000);
            }
        } else if (m_phase == 2) {
            

            const float duration = 300.0f;
            float progress = qBound(0.0f, float(m_elapsed.elapsed()) / duration, 1.0f);
            m_opacity = 1.0f - progress;
            if (progress >= 1.0f) {
                m_opacity = 0.0f;
                m_animTimer->stop();
                hide();
            }
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent*) override {
        if (m_opacity < 0.01f) return;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(m_opacity);

        

        QRectF r(0.5, 0.5, width()-1, height()-1);
        p.setBrush(QColor("#1E1E1E"));
        p.setPen(QPen(QColor("#3A3A3A"), 1.0));
        p.drawRoundedRect(r, 8.0, 8.0);

        

        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#1DB954"));
        p.drawEllipse(QPointF(22, height() * 0.5f), 7.f, 7.f);

        

        p.setPen(QPen(Qt::white, 1.8f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPointF check[3] = {
            {17.5f, float(height()) * 0.5f},
            {21.0f, float(height()) * 0.5f + 3.5f},
            {27.0f, float(height()) * 0.5f - 4.0f}
        };
        p.drawPolyline(check, 3);

        

        QFont f; f.setPixelSize(13); f.setFamily("Segoe UI");
        p.setFont(f);
        p.setPen(QColor("#F0F0F0"));
        p.drawText(QRectF(38, 0, width()-46, height()),
                   Qt::AlignVCenter | Qt::AlignLeft, m_msg);
    }

private:
    QString       m_msg;
    float         m_opacity = 1.0f;
    float         m_yPos    = 0.f;
    int           m_startY  = 0;
    int           m_endY    = 0;
    int           m_phase   = 0;   

    QTimer*       m_animTimer = nullptr;
    QTimer*       m_holdTimer = nullptr;
    QElapsedTimer m_elapsed;
};


class LogoWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogoWidget(QWidget* p = nullptr) : QWidget(p) { setFixedSize(100, 34); }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        

        QFont f; f.setPixelSize(15); f.setBold(true);
        f.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
        p.setFont(f); p.setPen(C_WHITE);
        p.drawText(rect(), Qt::AlignVCenter | Qt::AlignLeft, "GMacros");
    }
};


static void applyDarkTitleBar(QWidget* w) {
#ifdef _WIN32
    HWND hwnd = reinterpret_cast<HWND>(w->winId());
    BOOL dark = TRUE;
    

    if (FAILED(DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark))))
        DwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));
#else
    Q_UNUSED(w)
#endif
}


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    void showEvent(QShowEvent* e) override {
        QMainWindow::showEvent(e);
        applyDarkTitleBar(this);
    }

    void closeEvent(QCloseEvent* e) override {
        e->accept();
        if (m_dash) m_dash->stopHotkeyPoller();
        QApplication::quit();
    }

    void changeEvent(QEvent* e) override {
        if (e->type() == QEvent::WindowStateChange && isMinimized()) {
            e->ignore();
            hideWithAnimation();
            return;
        }
        QMainWindow::changeEvent(e);
    }

    void hideWithAnimation() {
        if (m_hiding) return;
        m_hiding = true;
        auto* anim = new QPropertyAnimation(this, "windowOpacity", this);
        anim->setDuration(200);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &QPropertyAnimation::finished, this, [this]{
            hide();
            setWindowOpacity(1.0);
            m_hiding = false;
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    void showWithAnimation() {
        setWindowOpacity(0.0);
        showNormal();
        raise();
        activateWindow();
        applyDarkTitleBar(this);
        auto* anim = new QPropertyAnimation(this, "windowOpacity", this);
        anim->setDuration(200);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("GMacros");
        setFixedSize(860, 640);
        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint
                           & ~Qt::WindowMinimizeButtonHint
                       | Qt::MSWindowsFixedSizeDialogHint);

        

        

        QIcon appIcon(":/logo/icon.ico");
        if (!appIcon.isNull()) {
            setWindowIcon(appIcon);
            QApplication::setWindowIcon(appIcon);
        }

        setStyleSheet(
            "QMainWindow { background:#141414; }"
            "QWidget { background:#141414; color:#F0F0F0;"
            "          font-family:'Segoe UI'; font-size:13px; }"
            "#topBar { background:#0F0F0F; border-bottom:1px solid #2E2E2E; }"
            "#botBar { background:#0F0F0F; border-top:1px solid #2E2E2E; }"
            "#btnApply {"
            "  background:#212121; color:#F0F0F0;"
            "  border:1px solid #3A3A3A; border-radius:6px; font-size:13px; }"
            "#btnApply:hover { background:#2C2C2C; border-color:#FFFFFF; }"
            "QStackedWidget { background:#141414; }"
            "QDialog { background:#1C1C1C; }"
            );
        auto* central = new QWidget(this);
        setCentralWidget(central);
        auto* root = new QVBoxLayout(central);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(0);

        

        auto* topBar = new QWidget; topBar->setObjectName("topBar");
        topBar->setFixedHeight(50);
        auto* tl = new QHBoxLayout(topBar);
        tl->setContentsMargins(14,0,14,0); tl->setSpacing(4);
        tl->addWidget(new LogoWidget(topBar));
        tl->addStretch();
        m_btnDash = new NavButton("DASHBOARD", topBar);
        tl->addWidget(m_btnDash);
        tl->addSpacing(6);
        root->addWidget(topBar);

        

        m_pages = new AnimatedStack;
        m_dash     = new DashboardPage;
        m_pages->addWidget(m_dash);
        root->addWidget(m_pages, 1);

        

        auto* botBar = new QWidget; botBar->setObjectName("botBar");
        botBar->setFixedHeight(50);
        auto* bl = new QHBoxLayout(botBar);
        bl->setContentsMargins(18,10,18,10); bl->addStretch();
        m_btnApply = new QPushButton("Apply");
        m_btnApply->setObjectName("btnApply");
        m_btnApply->setFixedSize(82,28);
        m_btnApply->setCursor(Qt::PointingHandCursor);
        bl->addWidget(m_btnApply);
        root->addWidget(botBar);

        

        

        m_snow = new SnowWidget(central);
        m_snow->setGeometry(0, 0, 860, 640);
        m_snow->raise(); 

        m_snow->show();

        m_btnDash->setActive(true);
        connect(m_btnDash, &QPushButton::clicked, this, [this]{ switchTab(0); });

        

        m_toast = new ToastNotification(central);
        connect(m_btnApply, &QPushButton::clicked, this, [this]{
            if (m_dash) m_dash->saveSettings();
            m_toast->popup("Settings applied successfully");
        });

        

        QIcon appIcon2(":/logo/icon.ico");
        m_tray = new QSystemTrayIcon(appIcon2.isNull() ? QIcon() : appIcon2, this);
        m_tray->setToolTip("GMacros");
        auto* trayMenu = new QMenu(this);
        trayMenu->setStyleSheet(
            "QMenu { background:#1C1C1C; color:#F0F0F0; border:1px solid #3A3A3A; }"
            "QMenu::item:selected { background:#2C2C2C; }"
            "QMenu::item { padding:6px 20px; font-family:'Segoe UI'; font-size:13px; }");
        auto* actShow = new QAction("Show", this);
        auto* actQuit = new QAction("Quit", this);
        trayMenu->addAction(actShow);
        trayMenu->addSeparator();
        trayMenu->addAction(actQuit);
        m_tray->setContextMenu(trayMenu);
        connect(actShow, &QAction::triggered, this, [this]{ showWithAnimation(); });
        connect(actQuit, &QAction::triggered, this, [this]{
            if (m_dash) m_dash->stopHotkeyPoller();
            QApplication::quit();
        });
        connect(m_tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason r){
            if (r == QSystemTrayIcon::DoubleClick || r == QSystemTrayIcon::Trigger)
                showWithAnimation();
        });
        m_tray->show();
    }

private slots:
    void switchTab(int i) {
        m_pages->switchTo(i);
        m_btnDash->setActive(i==0);
    }

private:
    NavButton*      m_btnDash  = nullptr;
    AnimatedStack*  m_pages    = nullptr;
    DashboardPage*  m_dash     = nullptr;
    QPushButton*      m_btnApply = nullptr;
    ToastNotification* m_toast   = nullptr;
    SnowWidget*          m_snow    = nullptr;
    QSystemTrayIcon*     m_tray    = nullptr;
    bool                 m_hiding  = false;
};


#include "main.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QFont f("Segoe UI", 10);
    if (!QFontDatabase::families().contains("Segoe UI"))
        f = QFont("Calibri", 10);
    app.setFont(f);
    MainWindow w;
    w.show();
    return app.exec();
}
