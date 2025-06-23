// SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#ifndef DRAWERCOURSETREEMODEL_H
#define DRAWERCOURSETREEMODEL_H

#include "core/iunit.h"
#include "isessionactions.h"
#include <QAbstractItemModel>
#include <QAction>
#include <QObject>
#include <QQmlEngine>
#include <optional>

#ifndef OPAQUE_PTR_Course
#define OPAQUE_PTR_Course
Q_DECLARE_OPAQUE_POINTER(ICourse *)
#endif

class SelectionEntry
{
public:
    explicit SelectionEntry() = default;
    explicit SelectionEntry(const QString &text, std::shared_ptr<IUnit> data, std::shared_ptr<SelectionEntry> parentItem);
    explicit SelectionEntry(const QString &text, std::shared_ptr<IPhrase> data, std::shared_ptr<SelectionEntry> parentItem);

    void appendChild(std::shared_ptr<SelectionEntry> child);

    std::shared_ptr<SelectionEntry> child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(Qt::ItemDataRole role) const;
    int row() const;
    std::shared_ptr<SelectionEntry> parentItem();
    std::shared_ptr<IPhrase> phrase();
    std::shared_ptr<IUnit> unit();

private:
    std::vector<std::shared_ptr<SelectionEntry>> m_childItems;
    std::weak_ptr<SelectionEntry> m_parentItem;
    QString m_text; //!< user formatted string
    QString m_toolTip;
    std::optional<std::shared_ptr<IUnit>> m_unit;
    std::optional<std::shared_ptr<IPhrase>> m_phrase;
};

class DrawerCourseTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(ICourse *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(ISessionActions *session READ session WRITE setSession NOTIFY sessionChanged)
    Q_PROPERTY(bool trainingFilter READ trainingFilter WRITE setTrainingFilter NOTIFY trainingFilterChanged)

    QML_ELEMENT

public:
    enum Roles {
        Text = Qt::DisplayRole,
        ToolTip = Qt::ToolTipRole,
    };
    Q_ENUM(Roles)

    /**
     * @brief Create filter tree model all units and phrases in a course
     *
     * @param parent the QObject parent
     */
    explicit DrawerCourseTreeModel(QObject *parent = nullptr);

    /**
     * @brief Destroys the object
     */
    ~DrawerCourseTreeModel() override;

    void setCourse(ICourse *course);
    ICourse *course() const;

    void setSession(ISessionActions *session);
    ISessionActions *session() const;

    /**
     * @brief training filter controls if empty units/phrases shall be hidden
     *
     * @note by chaning the training filter the model is rebuild. also an enabled
     *       training filter does not react to unit phrase data being changed.
     * @param trainingFilter the boolean value
     */
    void setTrainingFilter(bool trainingFilter);
    bool trainingFilter() const;

    /**
     * @copydoc QAbstractItemModel::rolesNames()
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @copydoc QAbstractItemModel::index()
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @copydoc QAbstractItemModel::parent()
     */
    QModelIndex parent(const QModelIndex &index) const override;

    /**
     * @copydoc QAbstractItemModel::rowCount()
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @copydoc QAbstractItemModel::columnCount()
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @copydoc QAbstractItemModel::data()
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Iteration operator that computes next index when traversing phrases
     * @return next phrase or invalid model index if none exists
     */
    QModelIndex next(const QModelIndex &index) const;

    /**
     * @brief trigger @p index as active phrase in session, if index points to phrase
     */
    void trigger(const QModelIndex &index);

Q_SIGNALS:
    void courseChanged();
    void sessionChanged();
    void trainingFilterChanged();
    /**
     * \brief forwarded signal of underlaying session that requests a changed currentIndex \p index
     */
    void currentIndexChanged(QModelIndex index);

private Q_SLOTS:
    void handleSessionPhraseChanged();

private:
    QModelIndex firstPhraseIndexInUnit(const QModelIndex &index) const;
    void rebuildModel();
    std::shared_ptr<SelectionEntry> m_rootItem;
    ICourse *m_course{nullptr};
    ISessionActions *m_session{nullptr};
    bool m_trainingFilter{true};
};

#endif
