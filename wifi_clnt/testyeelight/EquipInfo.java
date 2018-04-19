/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Transient;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "equip_info") 
@NamedQueries({
    @NamedQuery(name = "EquipInfo.findAll", query = "SELECT e FROM EquipInfo e"),
    @NamedQuery(name = "EquipInfo.findById", query = "SELECT e FROM EquipInfo e WHERE e.id = :id"),
    @NamedQuery(name = "EquipInfo.findByCode", query = "SELECT e FROM EquipInfo e WHERE e.code = :code"),
    @NamedQuery(name = "EquipInfo.findByName", query = "SELECT e FROM EquipInfo e WHERE e.name = :name"),
@NamedQuery(name = "EquipInfo.findByType", query = "SELECT e FROM EquipInfo e WHERE e.type = :type")})
public class EquipInfo implements Serializable {
    @Transient
    private PropertyChangeSupport changeSupport = new PropertyChangeSupport(this);
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "ID")
    private Integer id;
    @Basic(optional = false)
    @Column(name = "CODE")
    private String code;
    @Column(name = "NAME")
    private String name;
    @Column(name="TYPE")
    private String type;

    public EquipInfo() {
    }

    public EquipInfo(Integer id) {
        this.id = id;
    }

    public EquipInfo(Integer id, String code) {
        this.id = id;
        this.code = code;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        Integer oldId = this.id;
        this.id = id;
        changeSupport.firePropertyChange("id", oldId, id);
    }

    public String getCode() {
        return code;
    }

    public void setCode(String code) {
        String oldCode = this.code;
        this.code = code;
        changeSupport.firePropertyChange("code", oldCode, code);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        String oldName = this.name;
        this.name = name;
        changeSupport.firePropertyChange("name", oldName, name);
    }
    
    public String getType() {
        return type;
    }

    public void setType(String type) {
        String oldType = this.type;
        this.type=type;
        changeSupport.firePropertyChange("type", oldType, type);
    }
    
    @Override
    public int hashCode() {
        int hash = 0;
        hash += (id != null ? id.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof EquipInfo)) {
            return false;
        }
        EquipInfo other = (EquipInfo) object;
        if ((this.id == null && other.id != null) || (this.id != null && !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.EquipInfo[ id=" + id + " ]";
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.removePropertyChangeListener(listener);
    }
    
}
