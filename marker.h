#pragma once
#include <QAbstractListModel>
#include <vector>

class MarkerModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 1, StepRole, LatRole, LonRole };

    int rowCount(const QModelIndex& parent = {}) const override {
        return parent.isValid() ? 0 : int(m_items.size());
    }

    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || index.row() >= int(m_items.size()))
            return {};

        const Marker& m = m_items[index.row()];

        switch (role) {
        case NameRole: return m.name;
        case StepRole: return m.step;
        case LatRole:  return m.lat;
        case LonRole:  return m.lon;
        }
        return {};
    }

    QHash<int, QByteArray> roleNames() const override {
        return { {NameRole, "name"}, {StepRole,"step"}, {LatRole, "lat"}, {LonRole, "lon"}};
    }

    void addMarker(const QString& name, int step, double lat, double lon) {
        const int row = int(m_items.size());
        beginInsertRows({}, row, row);
        m_items.push_back({ name, step, lat, lon });
        endInsertRows();
    }

    void clear() {
        beginResetModel();
        m_items.clear();
        endResetModel();
    }

private:
    struct Marker { QString name; int step; double lat; double lon; };
    std::vector<Marker> m_items;
};