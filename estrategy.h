#ifndef ESTRATEGY_H
#define ESTRATEGY_H

#include <QThread>

class Estrategy : public QThread
{
    Q_OBJECT

signals:
    void enviarOperacao(QString ativof, float precof, int lotef, QString ladof);

public:
    explicit Estrategy(QObject *parent = nullptr);
    ~Estrategy();

    void run() override;




};

#endif // ESTRATEGY_H
