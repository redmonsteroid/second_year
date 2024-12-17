window.addEventListener('DOMContentLoaded', () => {
    const role = localStorage.getItem('role');
    if (role === 'teacher') {
      document.getElementById('editor-link').style.display = 'inline';
    }
  
    document.getElementById('logout-btn').addEventListener('click', () => {
      localStorage.clear();
      window.location.href = '../index.html';
    });
  });
  