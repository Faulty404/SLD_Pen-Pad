#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QToolBar>
#include <QAction>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextList>
#include <QInputDialog>
#include <QTextTable>
#include <QTextTableFormat>
#include <QPainter>
#include <QWidgetAction>
#include <QPainter>
#include <QPen>

class NotesApp : public QMainWindow {
    Q_OBJECT

public:
    NotesApp(QWidget *parent = nullptr) : QMainWindow(parent), ruledPage(false), gridPage(false), lightMode(true) {
        textEdit = new QTextEdit(this);
        setCentralWidget(textEdit);

        createMenus();
        createToolbar();

        setWindowTitle("Notes App");
        resize(800, 600);
        applyLightMode();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QMainWindow::paintEvent(event);

        if (ruledPage || gridPage) {
            QPainter painter(viewport()); // Use viewport for custom painting on the textEdit
            QPen pen(QColor(192, 192, 192)); // Light gray for ruled/grid lines
            painter.setPen(pen);

            if (ruledPage) {
                int lineHeight = 20; // Space between lines
                for (int y = lineHeight; y < height(); y += lineHeight) {
                    painter.drawLine(0, y, width(), y);
                }
            } else if (gridPage) {
                int gridSize = 20; // Space for grid cells
                for (int y = gridSize; y < height(); y += gridSize) {
                    painter.drawLine(0, y, width(), y); // Horizontal lines
                }
                for (int x = gridSize; x < width(); x += gridSize) {
                    painter.drawLine(x, 0, x, height()); // Vertical lines
                }
            }
        }
    }

private slots:
    void newNote() {
        textEdit->clear();
    }

    void openNote() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Note", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                textEdit->setText(in.readAll());
                file.close();
            } else {
                QMessageBox::warning(this, "Error", "Could not open file.");
            }
        }
    }

    void saveNote() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Note", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << textEdit->toPlainText();
                file.close();
            } else {
                QMessageBox::warning(this, "Error", "Could not save file.");
            }
        }
    }

    void quitApp() {
        QApplication::quit();
    }

    // Formatting slots
    void setBold() {
        QTextCharFormat format;
        format.setFontWeight(textEdit->fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
        applyFormat(format);
    }

    void setItalic() {
        QTextCharFormat format;
        format.setFontItalic(!textEdit->fontItalic());
        applyFormat(format);
    }

    void setUnderline() {
        QTextCharFormat format;
        format.setFontUnderline(!textEdit->fontUnderline());
        applyFormat(format);
    }

    void setTextColor() {
        QColor color = QColorDialog::getColor(textEdit->textColor(), this, "Choose Text Color");
        if (color.isValid()) {
            QTextCharFormat format;
            format.setForeground(color);
            applyFormat(format);
        }
    }

    void setFont() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, textEdit->currentFont(), this);
        if (ok) {
            QTextCharFormat format;
            format.setFont(font);
            applyFormat(format);
        }
    }

    void insertTable() {
        bool ok;
        int rows = QInputDialog::getInt(this, "Insert Table", "Number of rows:", 3, 1, 100, 1, &ok);
        if (!ok) return;
        int columns = QInputDialog::getInt(this, "Insert Table", "Number of columns:", 3, 1, 100, 1, &ok);
        if (!ok) return;

        QTextCursor cursor = textEdit->textCursor();
        QTextTableFormat tableFormat;
        tableFormat.setBorder(1);
        cursor.insertTable(rows, columns, tableFormat);
    }

    void insertBulletList() {
        QTextCursor cursor = textEdit->textCursor();
        cursor.insertList(QTextListFormat::ListDisc);
    }

    void insertNumberedList() {
        QTextCursor cursor = textEdit->textCursor();
        cursor.insertList(QTextListFormat::ListDecimal);
    }

    void insertEquation() {
        bool ok;
        QString equation = QInputDialog::getText(this, "Insert Equation", "Enter equation (LaTeX format):", QLineEdit::Normal, "", &ok);

        if (ok && !equation.isEmpty()) {
            textEdit->textCursor().insertText("$$ " + equation + " $$");
        }
    }

    void toggleRuledPage() {
        ruledPage = !ruledPage;
        gridPage = false; // Turn off grid if ruled is selected
        viewport()->update(); // Trigger repaint on the viewport
    }

    void toggleGridPage() {
        gridPage = !gridPage;
        ruledPage = false; // Turn off ruled if grid is selected
        viewport()->update(); // Trigger repaint on the viewport
    }

    void toggleLightMode() {
        lightMode = !lightMode;
        if (lightMode) {
            applyLightMode();
        } else {
            applyDarkMode();
        }
    }

private:
    QTextEdit *textEdit;
    bool ruledPage;
    bool gridPage;
    bool lightMode;

    void createMenus() {
        QMenu *fileMenu = menuBar()->addMenu("File");

        QAction *newAction = new QAction("New", this);
        connect(newAction, &QAction::triggered, this, &NotesApp::newNote);
        fileMenu->addAction(newAction);

        QAction *openAction = new QAction("Open", this);
        connect(openAction, &QAction::triggered, this, &NotesApp::openNote);
        fileMenu->addAction(openAction);

        QAction *saveAction = new QAction("Save", this);
        connect(saveAction, &QAction::triggered, this, &NotesApp::saveNote);
        fileMenu->addAction(saveAction);

        QAction *exitAction = new QAction("Exit", this);
        connect(exitAction, &QAction::triggered, this, &NotesApp::quitApp);
        fileMenu->addAction(exitAction);

        // Insert Menu
        QMenu *insertMenu = menuBar()->addMenu("Insert");

        QAction *insertTableAction = new QAction("Insert Table", this);
        connect(insertTableAction, &QAction::triggered, this, &NotesApp::insertTable);
        insertMenu->addAction(insertTableAction);

        QAction *insertBulletListAction = new QAction("Insert Bullet List", this);
        connect(insertBulletListAction, &QAction::triggered, this, &NotesApp::insertBulletList);
        insertMenu->addAction(insertBulletListAction);

        QAction *insertNumberedListAction = new QAction("Insert Numbered List", this);
        connect(insertNumberedListAction, &QAction::triggered, this, &NotesApp::insertNumberedList);
        insertMenu->addAction(insertNumberedListAction);

        QAction *insertEquationAction = new QAction("Insert Equation", this);
        connect(insertEquationAction, &QAction::triggered, this, &NotesApp::insertEquation);
        insertMenu->addAction(insertEquationAction);
    }

    void createToolbar() {
        QToolBar *toolbar = addToolBar("Text Formatting");

        QAction *boldAction = new QAction("Bold", this);
        boldAction->setCheckable(true);
        boldAction->setIcon(QIcon::fromTheme("format-text-bold"));
        connect(boldAction, &QAction::triggered, this, &NotesApp::setBold);
        toolbar->addAction(boldAction);

        QAction *italicAction = new QAction("Italic", this);
        italicAction->setCheckable(true);
        italicAction->setIcon(QIcon::fromTheme("format-text-italic"));
        connect(italicAction, &QAction::triggered, this, &NotesApp::setItalic);
        toolbar->addAction(italicAction);

        QAction *underlineAction = new QAction("Underline", this);
        underlineAction->setCheckable(true);
        underlineAction->setIcon(QIcon::fromTheme("format-text-underline"));
        connect(underlineAction, &QAction::triggered, this, &NotesApp::setUnderline);
        toolbar->addAction(underlineAction);

        QAction *textColorAction = new QAction("Text Color", this);
        textColorAction->setIcon(QIcon::fromTheme("format-text-color"));
        connect(textColorAction, &QAction::triggered, this, &NotesApp::setTextColor);
        toolbar->addAction(textColorAction);

        QAction *fontAction = new QAction("Font", this);
        fontAction->setIcon(QIcon::fromTheme("preferences-desktop-font"));
        connect(fontAction, &QAction::triggered, this, &NotesApp::setFont);
        toolbar->addAction(fontAction);

        toolbar->addSeparator();

        QAction *ruledPageAction = new QAction("Ruled Page", this);
        connect(ruledPageAction, &QAction::triggered, this, &NotesApp::toggleRuledPage);
        toolbar->addAction(ruledPageAction);

        QAction *gridPageAction = new QAction("Grid Page", this);
        connect(gridPageAction, &QAction::triggered, this, &NotesApp::toggleGridPage);
        toolbar->addAction(gridPageAction);

        QAction *lightModeAction = new QAction("Toggle Light/Dark Mode", this);
        connect(lightModeAction, &QAction::triggered, this, &NotesApp::toggleLightMode);
        toolbar->addAction(lightModeAction);
    }

    void applyLightMode() {
        QPalette palette = textEdit->palette();
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::Text, Qt::black);
        textEdit->setPalette(palette);
    }

    void applyDarkMode() {
        QPalette palette = textEdit->palette();
        palette.setColor(QPalette::Base, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        textEdit->setPalette(palette);
    }

    void applyFormat(const QTextCharFormat &format) {
        QTextCursor cursor = textEdit->textCursor();
        if (!cursor.hasSelection()) {
            cursor.select(QTextCursor::WordUnderCursor);
        }
        cursor.mergeCharFormat(format);
        textEdit->mergeCurrentCharFormat(format);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    NotesApp notesApp;
    notesApp.show();
    return app.exec();
}

#include "main.moc"
