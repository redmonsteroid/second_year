document.addEventListener("DOMContentLoaded", () => {
  // Считываем тему из localStorage
  const currentTheme = localStorage.getItem('theme');
  if (currentTheme === 'dark') {
    document.body.classList.add('dark-mode');
  }

  const sidebar = document.getElementById('sidebar');
  const menuToggle = document.getElementById('menu-toggle');
  const themeToggleMenu = document.getElementById('theme-toggle-menu');

  // ▼ Добавляем логику отображения ссылки на Teacher Panel
  const userRole = localStorage.getItem('role');
  if (userRole === 'teacher') {
    const sidebarUl = sidebar.querySelector('ul');
    if (sidebarUl) {
      const teacherLi = document.createElement('li');
      // Можно либо ссылкой на отдельную страницу, либо кнопкой
      teacherLi.innerHTML = `<a href="../pages/teacher_panel.html"><i class="fas fa-chalkboard-teacher"></i> Teacher Panel</a>`;
      sidebarUl.appendChild(teacherLi);
    }
  }

  if (menuToggle) {
    menuToggle.addEventListener('click', () => {
      sidebar.classList.toggle('open');
    });
  }

  if (themeToggleMenu) {
    themeToggleMenu.addEventListener('click', () => {
      document.body.classList.toggle('dark-mode');
      if (document.body.classList.contains('dark-mode')) {
        localStorage.setItem('theme', 'dark');
      } else {
        localStorage.setItem('theme', 'light');
      }
    });
  }

  // ▼ Логика закрытия меню при клике вне
  document.addEventListener('click', (event) => {
    if (!sidebar.classList.contains('open')) return;

    const clickInsideSidebar = sidebar.contains(event.target);
    const clickOnMenuButton = menuToggle.contains(event.target);

    if (!clickInsideSidebar && !clickOnMenuButton) {
      sidebar.classList.remove('open');
    }
  });
});
