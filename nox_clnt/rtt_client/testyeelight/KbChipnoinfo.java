/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.Serializable;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Transient;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "KB_CHIPNOINFO")
@NamedQueries({
    @NamedQuery(name = "KbChipnoinfo.findAll", query = "SELECT k FROM KbChipnoinfo k"),
    @NamedQuery(name = "KbChipnoinfo.findByChipno", query = "SELECT k FROM KbChipnoinfo k WHERE k.kbChipnoinfoPK.chipno = :chipno"),
    @NamedQuery(name = "KbChipnoinfo.findBySuser", query = "SELECT k FROM KbChipnoinfo k WHERE k.kbChipnoinfoPK.suser = :suser"),
    @NamedQuery(name = "KbChipnoinfo.findByDescription", query = "SELECT k FROM KbChipnoinfo k WHERE k.description = :description"),
    @NamedQuery(name = "KbChipnoinfo.findByDepartment", query = "SELECT k FROM KbChipnoinfo k WHERE k.department = :department"),
    @NamedQuery(name = "KbChipnoinfo.findByGrade", query = "SELECT k FROM KbChipnoinfo k WHERE k.grade = :grade")})
public class KbChipnoinfo implements Serializable {
    @Transient
    private PropertyChangeSupport changeSupport = new PropertyChangeSupport(this);
    private static final long serialVersionUID = 1L;
    @EmbeddedId
    protected KbChipnoinfoPK kbChipnoinfoPK;
    @Column(name = "DESCRIPTION")
    private String description;
    @Column(name = "DEPARTMENT")
    private String department;
    @Column(name = "GRADE")
    private String grade;

    public KbChipnoinfo() {
    }

    public KbChipnoinfo(KbChipnoinfoPK kbChipnoinfoPK) {
        this.kbChipnoinfoPK = kbChipnoinfoPK;
    }

    public KbChipnoinfo(String chipno, String suser) {
        this.kbChipnoinfoPK = new KbChipnoinfoPK(chipno, suser);
    }

    public KbChipnoinfoPK getKbChipnoinfoPK() {
        return kbChipnoinfoPK;
    }

    public void setKbChipnoinfoPK(KbChipnoinfoPK kbChipnoinfoPK) {
        this.kbChipnoinfoPK = kbChipnoinfoPK;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        String oldDescription = this.description;
        this.description = description;
        changeSupport.firePropertyChange("description", oldDescription, description);
    }

    public String getDepartment() {
        return department;
    }

    public void setDepartment(String department) {
        String oldDepartment = this.department;
        this.department = department;
        changeSupport.firePropertyChange("department", oldDepartment, department);
    }

    public String getGrade() {
        return grade;
    }

    public void setGrade(String grade) {
        String oldGrade = this.grade;
        this.grade = grade;
        changeSupport.firePropertyChange("grade", oldGrade, grade);
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (kbChipnoinfoPK != null ? kbChipnoinfoPK.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof KbChipnoinfo)) {
            return false;
        }
        KbChipnoinfo other = (KbChipnoinfo) object;
        if ((this.kbChipnoinfoPK == null && other.kbChipnoinfoPK != null) || (this.kbChipnoinfoPK != null && !this.kbChipnoinfoPK.equals(other.kbChipnoinfoPK))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.KbChipnoinfo[ kbChipnoinfoPK=" + kbChipnoinfoPK + " ]";
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.removePropertyChangeListener(listener);
    }
    
}
