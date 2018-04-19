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
@Table(name = "ttllf_cj")
@NamedQueries({
    @NamedQuery(name = "TtllfCj.findAll", query = "SELECT t FROM TtllfCj t"),
    @NamedQuery(name = "TtllfCj.findByPkCj", query = "SELECT t FROM TtllfCj t WHERE t.pkCj = :pkCj"),
    @NamedQuery(name = "TtllfCj.findByCode", query = "SELECT t FROM TtllfCj t WHERE t.code = :code"),
    @NamedQuery(name = "TtllfCj.findByName", query = "SELECT t FROM TtllfCj t WHERE t.name = :name"),
    @NamedQuery(name = "TtllfCj.findByPkGroup", query = "SELECT t FROM TtllfCj t WHERE t.pkGroup = :pkGroup"),
    @NamedQuery(name = "TtllfCj.findByPkOrg", query = "SELECT t FROM TtllfCj t WHERE t.pkOrg = :pkOrg"),
    @NamedQuery(name = "TtllfCj.findByPkOrgV", query = "SELECT t FROM TtllfCj t WHERE t.pkOrgV = :pkOrgV"),
    @NamedQuery(name = "TtllfCj.findByCreator", query = "SELECT t FROM TtllfCj t WHERE t.creator = :creator"),
    @NamedQuery(name = "TtllfCj.findByCreationtime", query = "SELECT t FROM TtllfCj t WHERE t.creationtime = :creationtime"),
    @NamedQuery(name = "TtllfCj.findByModifier", query = "SELECT t FROM TtllfCj t WHERE t.modifier = :modifier"),
    @NamedQuery(name = "TtllfCj.findByModifiedtime", query = "SELECT t FROM TtllfCj t WHERE t.modifiedtime = :modifiedtime"),
    @NamedQuery(name = "TtllfCj.findByTs", query = "SELECT t FROM TtllfCj t WHERE t.ts = :ts"),
    @NamedQuery(name = "TtllfCj.findByDr", query = "SELECT t FROM TtllfCj t WHERE t.dr = :dr")})
public class TtllfCj implements Serializable {
    @Transient
    private PropertyChangeSupport changeSupport = new PropertyChangeSupport(this);
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "pk_cj")
    private String pkCj;
    @Column(name = "code")
    private String code;
    @Column(name = "name")
    private String name;
    @Column(name = "pk_group")
    private String pkGroup;
    @Column(name = "pk_org")
    private String pkOrg;
    @Column(name = "pk_org_v")
    private String pkOrgV;
    @Column(name = "creator")
    private String creator;
    @Column(name = "creationtime")
    private String creationtime;
    @Column(name = "modifier")
    private String modifier;
    @Column(name = "modifiedtime")
    private String modifiedtime;
    @Column(name = "ts")
    private String ts;
    @Column(name = "dr")
    private Short dr;

    public TtllfCj() {
    }

    public TtllfCj(String pkCj) {
        this.pkCj = pkCj;
    }

    public String getPkCj() {
        return pkCj;
    }

    public void setPkCj(String pkCj) {
        String oldPkCj = this.pkCj;
        this.pkCj = pkCj;
        changeSupport.firePropertyChange("pkCj", oldPkCj, pkCj);
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

    public String getPkGroup() {
        return pkGroup;
    }

    public void setPkGroup(String pkGroup) {
        String oldPkGroup = this.pkGroup;
        this.pkGroup = pkGroup;
        changeSupport.firePropertyChange("pkGroup", oldPkGroup, pkGroup);
    }

    public String getPkOrg() {
        return pkOrg;
    }

    public void setPkOrg(String pkOrg) {
        String oldPkOrg = this.pkOrg;
        this.pkOrg = pkOrg;
        changeSupport.firePropertyChange("pkOrg", oldPkOrg, pkOrg);
    }

    public String getPkOrgV() {
        return pkOrgV;
    }

    public void setPkOrgV(String pkOrgV) {
        String oldPkOrgV = this.pkOrgV;
        this.pkOrgV = pkOrgV;
        changeSupport.firePropertyChange("pkOrgV", oldPkOrgV, pkOrgV);
    }

    public String getCreator() {
        return creator;
    }

    public void setCreator(String creator) {
        String oldCreator = this.creator;
        this.creator = creator;
        changeSupport.firePropertyChange("creator", oldCreator, creator);
    }

    public String getCreationtime() {
        return creationtime;
    }

    public void setCreationtime(String creationtime) {
        String oldCreationtime = this.creationtime;
        this.creationtime = creationtime;
        changeSupport.firePropertyChange("creationtime", oldCreationtime, creationtime);
    }

    public String getModifier() {
        return modifier;
    }

    public void setModifier(String modifier) {
        String oldModifier = this.modifier;
        this.modifier = modifier;
        changeSupport.firePropertyChange("modifier", oldModifier, modifier);
    }

    public String getModifiedtime() {
        return modifiedtime;
    }

    public void setModifiedtime(String modifiedtime) {
        String oldModifiedtime = this.modifiedtime;
        this.modifiedtime = modifiedtime;
        changeSupport.firePropertyChange("modifiedtime", oldModifiedtime, modifiedtime);
    }

    public String getTs() {
        return ts;
    }

    public void setTs(String ts) {
        String oldTs = this.ts;
        this.ts = ts;
        changeSupport.firePropertyChange("ts", oldTs, ts);
    }

    public Short getDr() {
        return dr;
    }

    public void setDr(Short dr) {
        Short oldDr = this.dr;
        this.dr = dr;
        changeSupport.firePropertyChange("dr", oldDr, dr);
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (pkCj != null ? pkCj.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof TtllfCj)) {
            return false;
        }
        TtllfCj other = (TtllfCj) object;
        if ((this.pkCj == null && other.pkCj != null) || (this.pkCj != null && !this.pkCj.equals(other.pkCj))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.TtllfCj[ pkCj=" + pkCj + " ]";
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.removePropertyChangeListener(listener);
    }
    
}
