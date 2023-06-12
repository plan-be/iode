#include "tab_text.h"


static const QStringList initialize_text_extensions()
{
    QStringList textExt;
    for(const std::string ext: v_text_ext)
        textExt << "." + QString::fromStdString(ext);
    return textExt;
}

const QStringList QIodeTextWidget::textExtensions = initialize_text_extensions();
